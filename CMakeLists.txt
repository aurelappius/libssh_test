cmake_minimum_required(VERSION 3.0.0)
project(libssh_test VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(libssh REQUIRED)

target_link_libraries(libssh_test ssh)
add_executable(libssh_test main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
