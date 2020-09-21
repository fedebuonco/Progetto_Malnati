cmake_minimum_required(VERSION 3.17)
project(client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)

include_directories(includes/connect_server)
include_directories(includes/transfer)
include_directories(includes/utilities)
include_directories(includes/watcher)
include_directories(includes/config)

add_executable(client src/main.cpp src/transfer/transfer.cpp includes/transfer/transfer.h src/connect_server/connect_server.cpp
        includes/connect_server/connect_server.h src/utilities/utilities.cpp includes/utilities/utilities.h src/watcher/watcher.cpp
        includes/watcher/watcher.h src/config/config.cpp includes/config/config.h)