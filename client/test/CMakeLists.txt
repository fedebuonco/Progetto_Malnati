cmake_minimum_required(VERSION 3.17)
project(test)
set(CMAKE_CXX_STANDARD 17)

# Various include files for our code
include_directories(../includes/sync_tcp_socket)
include_directories(../includes/transfer)
include_directories(../includes/utilities)
include_directories(../includes/watcher)
include_directories(../includes/config)
include_directories(../includes/client)

if (UNIX)
    set(CMAKE_CXX_FLAGS -pthread)
    include_directories(/usr/local/include/cryptopp)
    add_executable(test
            catch.hpp
            main_test.cpp
            ../includes/sync_tcp_socket/sync_tcp_socket.h
            ../src/sync_tcp_socket/sync_tcp_socket.cpp
            ../includes/utilities/utilities.h
            ../src/utilities/utilities.cpp
            ../includes/watcher/watcher.h
            ../src/watcher/watcher.cpp
            ../includes/config/config.h
            ../src/config/config.cpp
            ../includes/config/authentication.h
            ../src/config/authentication.cpp
            ../includes/transfer/control_message.h
            ../src/transfer/control_message.cpp
            ../includes/client/client.h
            ../src/client/client.cpp
            ../includes/transfer/tree_t.h
            ../src/transfer/tree_t.cpp
            ../includes/transfer/patch.h
            ../src/transfer/patch.cpp
            )
    target_link_libraries(test cryptopp)
endif (UNIX)


if (WIN32)
    #Per farlo funzionare esegui il file bat e poi il file b2.exe

    # Giving these hints to findboost (I still don't know why both are needed)
    set(Boost_LIBRARY_DIR C:/Boost/boost_1_74_0/stage/lib )    #non toccare!!!!!!!!!
    set(Boost_ROOT C:/Boost/boost_1_74_0)

    find_package(Boost 1.74.0)

    if(NOT Boost_FOUND)
        message(FATAL_ERROR "Boost Not found")
    endif()
    if(Boost_FOUND)
        # Let's see how the find package filled the variables
        message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
        message(STATUS "Boost_VERSION: ${Boost_VERSION}")

        # including in the project the Boost dir and giving to the linker the path where to look for libraries
        include_directories(${Boost_INCLUDE_DIR})
        link_directories(${Boost_LIBRARY_DIR})

        include_directories(../libs/cryptopp/include)
        unset(CRYPTOPP CACHE)

        find_library(CRYPTOPP cryptlib HINTS ../libs/cryptopp REQUIRED)
        if(NOT CRYPTOPP)
            message(FATAL_ERROR "Cryptopp Not found")
        endif()

        message(STATUS ${CRYPTOPP})

        add_executable(test
                catch.hpp
                main_test.cpp
                ../includes/sync_tcp_socket/sync_tcp_socket.h
                ../src/sync_tcp_socket/sync_tcp_socket.cpp
                ../includes/utilities/utilities.h
                ../src/utilities/utilities.cpp
                ../includes/watcher/watcher.h
                ../src/watcher/watcher.cpp
                ../includes/config/config.h
                ../src/config/config.cpp
                ../includes/config/authentication.h
                ../src/config/authentication.cpp
                ../includes/transfer/control_message.h
                ../src/transfer/control_message.cpp
                ../includes/client/client.h
                ../src/client/client.cpp
                ../includes/transfer/tree_t.h
                ../src/transfer/tree_t.cpp
                ../includes/transfer/patch.h
                ../src/transfer/patch.cpp
                )

        target_link_libraries(test ${CRYPTOPP})
    endif()
endif (WIN32)





