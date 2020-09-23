cmake_minimum_required(VERSION 3.17)
project(client)
set(CMAKE_CXX_STANDARD 17)
SET( CMAKE_C_FLAGS "-Wall -q" )     #Mi permette di avere delle stampe di debug
include_directories(includes/connect_server)
include_directories(includes/transfer)
include_directories(includes/utilities)
include_directories(includes/watcher)
include_directories(includes/config)


if (UNIX)
    set(CMAKE_CXX_FLAGS -pthread)
    add_executable(client src/main.cpp src/transfer/transfer.cpp includes/transfer/transfer.h src/connect_server/connect_server.cpp
            includes/connect_server/connect_server.h src/utilities/utilities.cpp includes/utilities/utilities.h src/watcher/watcher.cpp
            includes/watcher/watcher.h src/config/config.cpp includes/config/config.h)
endif (UNIX)

if (WIN32)
    #Per farlo funzionare esegui il file bat e poi il file b2.exe
    set( Boost_LIBRARY_DIR C:/Boost/boost_1_74_0/stage/lib )    #non toccare!!!!!!!!!
    set(Boost_ROOT C:/Boost/boost_1_74_0)
    find_package(Boost 1.74.0)

    if(NOT Boost_FOUND)
        message(FATAL_ERROR "Boost Not found")
    endif()
    if(Boost_FOUND)
        message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
        #message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}") VUOTO
        message(STATUS "Boost_VERSION: ${Boost_VERSION}")
        include_directories(${Boost_INCLUDE_DIR})
        link_directories(${Boost_LIBRARY_DIR})


        add_executable(client src/main.cpp src/transfer/transfer.cpp includes/transfer/transfer.h src/connect_server/connect_server.cpp
                includes/connect_server/connect_server.h src/utilities/utilities.cpp includes/utilities/utilities.h src/watcher/watcher.cpp
                includes/watcher/watcher.h src/config/config.cpp includes/config/config.h)



        #target_link_libraries(client ${Boost_LIBRARY_DIR} )
    endif()
endif (WIN32)