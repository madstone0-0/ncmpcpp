cmake_minimum_required(VERSION 3.14)
project(ncmpcpp LANGUAGES CXX VERSION "0.10")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

option(ENABLE_OUTPUTS "Enable outputs screen" OFF)
if(ENABLE_OUTPUTS)
    add_definitions(-DENABLE_OUTPUTS=1)
endif()


option(ENABLE_CLOCK "Enable clock screen" OFF)
if(ENABLE_CLOCK)
    add_definitions(-DENABLE_CLOCK=1)
endif()

find_package(PkgConfig REQUIRED)

option(ENABLE_VISUALIZER "Enable music visualizer screen" OFF)
option(WITH_FFTW "Enable fftw support required for frequencey specturm vizualization" ON)
if(WITH_FFTW AND ENABLE_VISUALIZER)
    set(VISUALIZER 1)
    add_definitions(-DWITH_FFTW)
    pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)
    include_directories(PkgConfig::FFTW)
    link_libraries(PkgConfig::FFTW)
    add_definitions(-DENABLE_VISUALIZER=1)
elseif(ENABLE_VISUALIZER)
    message(ERROR "fftw3 library is required!")
endif()

option(WITH_TAGLIB "Enable tag editor" ON)
if(WITH_TAGLIB)
    add_definitions(-DWITH_TAGLIB)
    pkg_search_module(TAGLIB REQUIRED taglib IMPORTED_TARGET)
    include_directories(PkgConfig::TAGLIB)
    link_libraries(PkgConfig::TAGLIB)
endif()

option(WITH_LTO "Enable LTO (link time optimization)" OFF)
if(WITH_LTO)
    add_definitions(-DWITH_LTO)
    set(CMAKE_CXX_FLAGS "-flto")
endif()



# set(CMAKE_CXX_FLAGS "-Wall -Wextra -ltag -lmpdclient -ltbb -lpthread -lreadline -lfftw3 -licudata -licui18n -licuio -licuuc")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -lpthread -licudata -licui18n -licuio -licuuc")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Os -fno-ident -ftree-vectorize -ffast-math")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

# include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR})

pkg_search_module(MPDCLIENT REQUIRED libmpdclient IMPORTED_TARGET)
include_directories(PkgConfig::MPDCLIENT)
link_libraries(PkgConfig::MPDCLIENT)

pkg_search_module(READLINE REQUIRED readline IMPORTED_TARGET )
include_directories(PkgConfig::READLINE)
link_libraries(PkgConfig::READLINE)


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

configure_file(config.cmake.h.in config.h)
add_subdirectory(src)
