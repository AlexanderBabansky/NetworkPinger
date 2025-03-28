project(microservice)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

find_package(rabbitmq-c REQUIRED)

add_executable(${PROJECT_NAME} microservice.cpp AMQPInstance.h AMQPInstance.cpp AMQPStructs.h AMQPStructs.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE rabbitmq::rabbitmq-static nlohmann_json::nlohmann_json network core)
