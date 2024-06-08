cmake_minimum_required(VERSION 3.26)
set(PROJECTNAME "AudioLooper")
project(${PROJECTNAME})

## Helper Functions
#####################################################
include(config/cmake/helpers.cmake)

set(SFML_STATIC_LIBRARIES TRUE)
set(BUILD_SHARED_LIBS OFF)

## Other variables and compiler setup I
#####################################################

if (MSVC)
    add_compile_definitions(NOMINMAX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20")
endif ()

# set cxx standard
########################################

set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD 20)

## get externals
#####################################################

add_subdirectory(ext)

setup_sfml()

add_subdirectory(impl)

## copy .clang-format to build directory
#####################################################
configure_file(${CMAKE_SOURCE_DIR}/.clang-format ${CMAKE_BINARY_DIR}/ COPYONLY)
