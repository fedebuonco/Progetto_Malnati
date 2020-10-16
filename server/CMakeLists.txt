cmake_minimum_required(VERSION 3.17)
project(server)

SET( CMAKE_C_FLAGS "-Wall -q" )     #Mi permette di avere delle stampe di debug
set(CMAKE_CXX_STANDARD 17)

include_directories(includes/accept_client)
include_directories(includes/transfer)
include_directories(includes/utilities)
include_directories(includes/watcher)




#--------------------------------------------------------------------

#--------UNIX-------------------
if (UNIX)
    set(CMAKE_CXX_FLAGS -pthread)
    add_executable(server src/main.cpp
            src/accept_client/accept_client.cpp includes/accept_client/accept_client.h
            src/watcher/watcher.cpp includes/watcher/watcher.h src/accept_client/service.cpp
            includes/accept_client/service.h src/accept_client/server.cpp includes/accept_client/server.h src/transfer/control_message.cpp
            includes/transfer/control_message.h)
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

        add_executable(server src/main.cpp
                src/accept_client/accept_client.cpp includes/accept_client/accept_client.h
                src/watcher/watcher.cpp includes/watcher/watcher.h src/accept_client/service.cpp includes/accept_client/service.h
                src/accept_client/server.cpp
                includes/accept_client/server.h src/transfer/control_message.cpp includes/transfer/control_message.h)

    endif()
endif (WIN32)