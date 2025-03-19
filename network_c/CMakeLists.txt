project(network_c)

add_library(${PROJECT_NAME} SHARED
    NetworkPingerMulithread_c.cpp NetworkPingerMulithread_c.h
)
target_link_libraries(${PROJECT_NAME} PRIVATE network)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
if (WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()