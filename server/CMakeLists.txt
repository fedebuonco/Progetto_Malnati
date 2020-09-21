cmake_minimum_required(VERSION 3.17)
project(server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)
add_executable(server src/main.cpp config/ConfigClass.cpp config/ConfigClass.h src/utilities/ut.cpp src/utilities/ut.h)