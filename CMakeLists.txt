cmake_minimum_required(VERSION 4.2)

project(FixEngine VERSION 0.1.0 LANGUAGES CXX)

# Build type
set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)

# Global warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# Global output folders (NO need to repeat anywhere)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)  # .a / .lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)  # .so / .dylib
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)  # executables

# Set C++ standard globally
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# TODO: Remove default Debug build before release
# If no build type is specified, default to Debug for development.
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No CMAKE_BUILD_TYPE specified. Defaulting to Debug.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Suggested by chatgpt to enable debugging in vs-code
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Recursively add submodules under Src/
file(GLOB_RECURSE _cmake_files
     RELATIVE "${CMAKE_SOURCE_DIR}"
     "${CMAKE_SOURCE_DIR}/Src/**/CMakeLists.txt")

foreach(_cmake IN LISTS _cmake_files)
    get_filename_component(_dir "${_cmake}" DIRECTORY)
    message(STATUS "Adding subdirectory: ${_dir}")
    add_subdirectory("${_dir}")
endforeach()



