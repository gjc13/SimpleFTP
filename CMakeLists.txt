cmake_minimum_required(VERSION 3.2)
project(SimpleFTP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES net_helper.cpp Client.cpp Server.cpp)

add_executable(simpleftp_client ${SOURCE_FILES} run_client.cpp)
add_executable(simpleftp_server ${SOURCE_FILES} run_server.cpp)
