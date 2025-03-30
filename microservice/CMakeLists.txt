project(NetworkPingerService)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)


find_package(rabbitmq-c REQUIRED)

add_library(AMQPUtils STATIC AMQPInstance.h AMQPInstance.cpp AMQPStructs.h AMQPStructs.cpp)
target_link_libraries(AMQPUtils PRIVATE nlohmann_json::nlohmann_json network stdlogger core)
target_link_libraries(AMQPUtils PUBLIC rabbitmq::rabbitmq-static)

add_executable(${PROJECT_NAME} microservice_cli.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE AMQPUtils stdlogger core)
