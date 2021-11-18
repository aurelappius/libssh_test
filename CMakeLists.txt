cmake_minimum_required(VERSION 3.16)
project(libssh_test VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)


find_package(libssh)

add_executable(libssh_test main.cpp)
target_link_libraries(libssh_test PRIVATE ssh)

# add_subdirectory(external)