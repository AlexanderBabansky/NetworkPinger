project(network)
add_library(${PROJECT_NAME} STATIC
    NetworkPinger.h
    NetworkPingerMulithread.h NetworkPingerMulithread.cpp
)
if(WIN32)
    target_sources(${PROJECT_NAME} PRIVATE
        win32/NetworkPingerWin32.cpp
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE Iphlpapi.lib ws2_32.lib)
else()
    target_sources(${PROJECT_NAME} PRIVATE
        unix/NetworkPingerUnix.cpp
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE core)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
