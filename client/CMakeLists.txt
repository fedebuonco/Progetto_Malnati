cmake_minimum_required(VERSION 3.17)
project(client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)

include_directories(includes/connecttoserver)
include_directories(includes/transfer)
include_directories(includes/utilities)
include_directories(includes/watcher)

add_executable(client src/main.cpp src/transfer/transfer.cpp includes/transfer/transfer.h src/connecttoserver/connectToServer.cpp includes/connecttoserver/connectToServer.h src/utilities/utilities.cpp includes/utilities/utilities.h src/watcher/watcher.cpp includes/watcher/watcher.h)