project(core)
add_library(${PROJECT_NAME} STATIC
    OnExit.h OnExit.cpp
    NMTimer.h NMTimer.cpp
    utils.cpp utils.h
)
target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
