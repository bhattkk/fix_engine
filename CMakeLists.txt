cmake_minimum_required(VERSION 3.20)

# Project Name , version and languages
project(FixEngine VERSION 0.1.0 LANGUAGES CXX)

# Global options
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Global warnings profile
if(MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Recursively add all subdirectories containing CMakeLists.txt under Src
file(GLOB_RECURSE _cmake_files
     RELATIVE "${CMAKE_SOURCE_DIR}"
     "${CMAKE_SOURCE_DIR}/Src/**/CMakeLists.txt")

foreach(_cmake IN LISTS _cmake_files)
    get_filename_component(_dir "${_cmake}" DIRECTORY)
    message(STATUS "Adding subdirectory: ${_dir}")
    add_subdirectory("${_dir}")
endforeach()

