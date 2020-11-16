cmake_minimum_required(VERSION 3.17)
project(client)
set(CMAKE_CXX_STANDARD 17)
SET( CMAKE_C_FLAGS "-Wall -q" )     #Mi permette di avere delle stampe di debug

# Various include files for our code
include_directories(includes/sync_tcp_socket)
include_directories(includes/transfer)
include_directories(includes/utilities)
include_directories(includes/watcher)
include_directories(includes/config)



#--------UNIX-------------------
if (UNIX)
    set(CMAKE_CXX_FLAGS -pthread)
    #include_directories(usr/local/lib)
    include_directories(/usr/local/include/cryptopp)
    add_executable(client src/main.cpp src/sync_tcp_socket/sync_tcp_socket.cpp
            includes/sync_tcp_socket/sync_tcp_socket.h src/utilities/utilities.cpp includes/utilities/utilities.h src/watcher/watcher.cpp
            includes/watcher/watcher.h src/config/config.cpp includes/config/config.h src/config/authentication.cpp includes/config/authentication.h
            src/transfer/control_message.cpp includes/transfer/control_message.h src/client/client.cpp includes/client/client.h src/transfer/tree_t.cpp includes/transfer/tree_t.h src/transfer/patch.cpp includes/transfer/patch.h)
    target_link_libraries(client cryptopp)
endif (UNIX)

#-------WIN---------------------
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

        include_directories(libs/cryptopp/include)
        unset(CRYPTOPP CACHE)

        find_library(CRYPTOPP cryptlib HINTS libs/cryptopp REQUIRED)
        if(NOT CRYPTOPP)
            message(FATAL_ERROR "Cryptopp Not found")
        endif()

        message(STATUS ${CRYPTOPP})


        add_executable(client src/main.cpp src/sync_tcp_socket/sync_tcp_socket.cpp
                includes/sync_tcp_socket/sync_tcp_socket.h src/utilities/utilities.cpp includes/utilities/utilities.h src/watcher/watcher.cpp
                includes/watcher/watcher.h src/config/config.cpp includes/config/config.h src/config/authentication.cpp includes/config/authentication.h
                src/transfer/control_message.cpp includes/transfer/control_message.h src/client/client.cpp includes/client/client.h src/transfer/tree_t.cpp includes/transfer/tree_t.h src/transfer/patch.cpp includes/transfer/patch.h)

        target_link_libraries(client ${CRYPTOPP})

    endif()
endif (WIN32)