#include "AMQPInstance.h"
#ifdef _WIN32
#include "winsock2.h"
#endif

#include <cstdarg>
#include <cstdio>
#include <vector>
#include <cstring>

#include "NMLogger.h"

namespace {
constexpr int CHANNEL_ID = 1;

struct timeval create_timeval_from_ms(long milliseconds)
{
    struct timeval tv;
    tv.tv_sec = milliseconds / 1000;           // Convert milliseconds to seconds
    tv.tv_usec = (milliseconds % 1000) * 1000; // Remaining milliseconds to microseconds
    return tv;
}

} // namespace
std::shared_ptr<AMQPInstance> AMQPInstance::create(const char *hostname, int port,
                                                   const char *username, const char *password)
{
    int status = 0;
    amqp_rpc_reply_t reply{};
    auto result = std::shared_ptr<AMQPInstance>(new AMQPInstance());

    result->conn = amqp_new_connection();
    if (!result->conn) {
        RE_LOG_ERROR("Failed to create AMQP connection");
        return {};
    }

    result->socket = amqp_tcp_socket_new(result->conn);
    if (!result->socket) {
        RE_LOG_ERROR("Failed to create TCP socket");
        return {};
    }

    status = amqp_socket_open(result->socket, hostname, port);
    if (status) {
        RE_LOG_ERROR("opening TCP socket %s:%d", hostname, port);
        return {};
    }
    reply = amqp_login(result->conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, username, password);
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        RE_LOG_ERROR("Failed to login AMQP");
        return {};
    }

    amqp_channel_open(result->conn, CHANNEL_ID);
    reply = amqp_get_rpc_reply(result->conn);
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        RE_LOG_ERROR("Failed to amqp_channel_open");
        return {};
    }
    result->mChOpened = true;

    return result;
}
// namespace

AMQPInstance::AMQPInstance() {}

AMQPInstance::~AMQPInstance()
{
    if (!conn) {
        return;
    }
    if (mChOpened) {
        amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
    }

    if (!mClosed) {
        amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
    }
    amqp_destroy_connection(conn);
}

bool AMQPInstance::declareQueue(const char *queueName)
{
    amqp_rpc_reply_t reply{};
    amqp_queue_declare(conn, CHANNEL_ID, amqp_cstring_bytes(queueName), 0, 0, 0, 0,
                       amqp_empty_table);
    reply = amqp_get_rpc_reply(conn);
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        RE_LOG_ERROR("Failed to amqp_queue_declare %s", queueName);
        return false;
    }
    return true;
}

bool AMQPInstance::startConsumeQueue(const char *queueName)
{
    if (!declareQueue(queueName)) {
        return false;
    }
    amqp_rpc_reply_t reply{};
    amqp_basic_consume(conn, CHANNEL_ID, amqp_cstring_bytes(queueName), amqp_empty_bytes, 0, 1, 0,
                       amqp_empty_table);
    reply = amqp_get_rpc_reply(conn);
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        RE_LOG_ERROR("Failed to amqp_basic_consume queue %s", queueName);
        return false;
    }
    return true;
}

std::string AMQPInstance::receiveMessage()
{
    timeval timeout = create_timeval_from_ms(10);

    while (mShouldStop == false) {
        amqp_maybe_release_buffers(conn);
        amqp_rpc_reply_t res{};
        amqp_envelope_t envelope{0};
        res = amqp_consume_message(conn, &envelope, &timeout, 0);

        if (AMQP_RESPONSE_NORMAL != res.reply_type && res.library_error == AMQP_STATUS_TIMEOUT) {
            continue;
        }
        if (AMQP_RESPONSE_NORMAL != res.reply_type) {
            RE_LOG_ERROR("Failed to amqp_consume_message");
            return {};
        }
        std::string envelopeVec;
        envelopeVec.resize(envelope.message.body.len);
        memcpy(envelopeVec.data(), envelope.message.body.bytes, envelope.message.body.len);

        amqp_destroy_envelope(&envelope);
        return envelopeVec;
    }
    return {};
}

bool AMQPInstance::sendString(const char *queueName, const std::string &msg)
{
    int status = 0;
    amqp_basic_properties_t props{0};
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.content_type = amqp_cstring_bytes("text/plain");
    props.delivery_mode = 2; /* persistent delivery mode */
    status = amqp_basic_publish(conn, CHANNEL_ID, amqp_empty_bytes, amqp_cstring_bytes(queueName),
                                0, 0, &props, amqp_cstring_bytes(msg.c_str()));
    if (status) {
        RE_LOG_ERROR("Failed to amqp_basic_publish to queue %s", queueName);
        return false;
    }
    return true;
}

void AMQPInstance::stop() { mShouldStop = true; }
