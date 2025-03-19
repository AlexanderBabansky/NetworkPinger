project(core)
add_library(${PROJECT_NAME} STATIC
    OnExit.h OnExit.cpp
    NMTimer.h NMTimer.cpp
)
target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})