cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("NetworkPinger")

add_library(core STATIC OnExit.h OnExit.cpp NMTimer.h NMTimer.cpp)
target_compile_definitions(core PRIVATE NOMINMAX)

add_library(network STATIC
    NetworkPinger.h
    NetworkPingerMulithread.cpp NetworkPingerMulithread.h
    )
target_link_libraries(network PRIVATE core)
if (WIN32)    
    target_sources(network PRIVATE NetworkPingerWin32.cpp)
    target_link_libraries(network PRIVATE Iphlpapi.lib ws2_32.lib)
else()
    target_sources(network PRIVATE NetworkPingerUnix.cpp)
endif()

# Add source to this project's executable.
add_executable (NetworkPinger main.cpp)
target_link_libraries(NetworkPinger PRIVATE network)

add_executable(Service service.cpp sqlite3.c sqlite3.h sqlite3ext.h
    DBManager.cpp DBManager.h
)
target_link_libraries(Service PRIVATE network)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET NetworkPinger PROPERTY CXX_STANDARD 20)
endif()


