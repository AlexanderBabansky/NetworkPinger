cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("NetworkPinger")
set(CMAKE_CXX_STANDARD 20 REQUIRED)
SET(CMAKE_PREFIX_PATH
        ${CMAKE_BINARY_DIR}
        ${CMAKE_PREFIX_PATH}
        )
option(BUILD_TESTS OFF)

add_subdirectory(core)
add_subdirectory(stdlogger)
if (WIN32)
    add_subdirectory(win32service)
endif()
add_subdirectory(network)
if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
add_subdirectory(microservice)
