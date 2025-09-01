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

# Scan Src/* and add any subdir that contains a CMakeLists.txt
file(GLOB _children LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/Src/*")
foreach(_path IN LISTS _children)
  if(EXISTS "${_path}/CMakeLists.txt")
    get_filename_component(_name "${_path}" NAME)
    message(STATUS "Adding subdirectory: Src/${_name}")
    add_subdirectory("Src/${_name}")
  endif()
endforeach()
