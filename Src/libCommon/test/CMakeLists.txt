include(FetchContent)

# -------------------------
# Google Test
# -------------------------

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Create test executables for all the files inside this directory
file(GLOB TEST_SOURCES "*.test.cpp"
)
foreach(TEST_SOURCE ${TEST_SOURCES})
  # Get the filename with .cpp removed
  get_filename_component(FULL_NAME ${TEST_SOURCE} NAME)
  string(REGEX REPLACE "\\.cpp$" "" TEST_NAME "${FULL_NAME}")

  # Create an executable for each test source file
  add_executable(${TEST_NAME} ${TEST_SOURCE})

  target_compile_options(${TEST_NAME} PRIVATE -fsanitize=thread -g)
  target_link_options(${TEST_NAME} PRIVATE -fsanitize=thread)

  # Link the test executable to the Common library and gtest libraries
  target_link_libraries(${TEST_NAME} PRIVATE Common gtest gtest_main)

endforeach()


# -------------------------
# Google Benchmark
# -------------------------
FetchContent_Declare(
    googlebenchmark
    URL https://github.com/google/benchmark/archive/refs/heads/main.zip
)

# Disable tests inside benchmark library (faster build)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googlebenchmark)

# Create benchmark executables for all the files inside this directory
file(GLOB BENCHMARK_SOURCES "*.bench.cpp")

foreach(BENCH_SOURCE ${BENCHMARK_SOURCES})
  get_filename_component(FULL_NAME ${BENCH_SOURCE} NAME)
  string(REGEX REPLACE "\\.cpp$" "" BENCH_NAME "${FULL_NAME}")

  add_executable(${BENCH_NAME} ${BENCH_SOURCE})

  # Link with your Common library + Google Benchmark
  target_link_libraries(${BENCH_NAME} PRIVATE Common benchmark::benchmark)

  # Force optimization only for this target
  target_compile_options(${BENCH_NAME} PRIVATE -O3 -DNDEBUG)
  target_link_options(${BENCH_NAME} PRIVATE -O3)
endforeach()
