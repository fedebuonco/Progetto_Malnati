cmake_minimum_required(VERSION 3.17)
project(server)


include_directories(includes/accept_client)
include_directories(includes/transfer)
include_directories(includes/utilities)
include_directories(includes/watcher)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)
add_executable(server src/main.cpp
        src/accept_client/accept_client.cpp includes/accept_client/accept_client.h
        src/transfer/transfer.cpp includes/transfer/transfer.h
        src/watcher/watcher.cpp includes/watcher/watcher.h  )