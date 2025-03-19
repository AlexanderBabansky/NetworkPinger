project(network_c)

add_library(${PROJECT_NAME} SHARED
    NetworkPingerMulithread_c.cpp NetworkPingerMulithread_c.h
)
target_link_libraries(network_c PRIVATE network)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(network_c PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
