#pragma once
#include "rabbitmq-c/amqp.h"
#include "rabbitmq-c/tcp_socket.h"
#include <memory>
#include <string>
#include <atomic>

class AMQPInstance
{
public:
    static std::shared_ptr<AMQPInstance> create(const char *hostname, int port,
                                                const char *username, const char *password);

    AMQPInstance();
    ~AMQPInstance();

    bool declareQueue(const char *queueName);
    bool startConsumeQueue(const char *queueName);
    std::string receiveMessage();

    bool sendString(const char *queueName, const std::string &msg);

    void stop();

private:
    amqp_connection_state_t conn = NULL;
    amqp_socket_t *socket = NULL;
    bool mClosed = false;

    bool mChOpened = false;

    std::atomic_bool mShouldStop = false;
};
