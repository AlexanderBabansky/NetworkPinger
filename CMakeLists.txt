cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("NetworkPinger")
set(CMAKE_CXX_STANDARD 20 REQUIRED)
SET(CMAKE_PREFIX_PATH
        ${CMAKE_BINARY_DIR}/conan_packages
        ${CMAKE_PREFIX_PATH}
        )

add_subdirectory(core)
add_subdirectory(network)
add_subdirectory(network_c)
add_subdirectory(tests)
add_subdirectory(microservice)

