cmake_minimum_required(VERSION 3.10)
project(lab1_skel C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(server src/server.c src/tcp.c)
add_executable(client src/client.c src/tcp.c)

include_directories(include)
