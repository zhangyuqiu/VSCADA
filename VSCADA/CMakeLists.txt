cmake_minimum_required (VERSION 2.8)
project (VSCADA)

#version number
set (VSCADA_VERSION_MAJOR 1)
set (VSCADA_VERSION_MINOR 0)

#include the subdirectory containing our libs
add_subdirectory (build-Configuration-Desktop_Qt_5_9_1_GCC_64bit-Debug)
include_directories(build-Configuration-Desktop_Qt_5_9_1_GCC_64bit-Debug)
#indicate the entry point for the executable
add_executable (VSCADA main.cpp)

# Indicate which libraries to include during the link process.
target_link_libraries (VSCADA)

install (TARGETS VSCADA DESTINATION bin)
