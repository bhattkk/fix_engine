include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Create test executables for all the files inside this directory
file(GLOB TEST_SOURCES "*.cpp" "*.cxx" "*.cc" "*.c"
)
foreach(TEST_SOURCE ${TEST_SOURCES})
  # Get the filename without extension
  get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

  # Create an executable for each test source file
  add_executable(${TEST_NAME} ${TEST_SOURCE})

  # Link the test executable to the Common library and gtest libraries
  target_link_libraries(${TEST_NAME} PRIVATE Common gtest gtest_main)

endforeach()
