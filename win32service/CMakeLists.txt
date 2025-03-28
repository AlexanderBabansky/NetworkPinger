project(win32service)

add_library(${PROJECT_NAME} STATIC service_wrapper.cpp win32logger.cpp NMLogger.h win32logger.h)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})