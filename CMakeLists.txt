cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("NetworkPinger")

# Add source to this project's executable.
add_executable (NetworkPinger main.cpp NetworkPinger.h OnExit.h OnExit.cpp NMTimer.h NMTimer.cpp)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET NetworkPinger PROPERTY CXX_STANDARD 20)
endif()

if (WIN32)
    target_compile_definitions(NetworkPinger PRIVATE NOMINMAX)
    target_sources(NetworkPinger PRIVATE NetworkPingerWin32.cpp)
    target_link_libraries(NetworkPinger PRIVATE Iphlpapi.lib ws2_32.lib)
else()
    target_sources(NetworkPinger PRIVATE NetworkPingerUnix.cpp)
endif()
