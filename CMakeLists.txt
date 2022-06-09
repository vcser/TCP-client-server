cmake_minimum_required(VERSION 3.10)
project(lab1_redes C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

link_libraries(-lsodium)

add_executable(server src/server.c src/tcp.c src/crypto.c src/util.c)
add_executable(client src/client.c src/tcp.c src/crypto.c src/util.c)
add_executable(keygen src/keygen.c)

include_directories(include)
