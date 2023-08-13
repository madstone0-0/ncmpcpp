cmake_minimum_required(VERSION 3.14)
project(ncmpcpp LANGUAGES CXX VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()


set(CMAKE_CXX_FLAGS "-Wall -Wextra -ltag -lmpdclient -ltbb -lpthread -lreadline -lfftw3 -licudata -licui18n -licuio -licuuc")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Os -fno-ident")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src /usr/include/taglib)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

#################
# Boost Library #
#################
set( Boost_USE_STATIC_LIBS OFF )
set(Boost_USE_MULTITHREADED ON)
set( Boost_USE_STATIC_RUNTIME OFF )
find_package(Boost COMPONENTS regex date_time system filesystem program_options thread locale REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_subdirectory(src)
