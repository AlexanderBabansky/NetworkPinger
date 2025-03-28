project(stdlogger)

add_library(${PROJECT_NAME} STATIC stdlogger.cpp NMLogger.h)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
