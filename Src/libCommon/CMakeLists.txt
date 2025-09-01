file(GLOB SRC_FILES "*.cpp")

add_library(libCommon ${SRC_FILES})

target_include_directories(libCommon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# Require C++17 for this target
target_compile_features(libCommon PUBLIC cxx_std_17)

# Link to header-only deps
target_link_libraries(libCommon
  PUBLIC
    Interfaces
)

# If you want to avoid the double "lib" prefix on Unix (liblibCommon.a),
# rename the OUTPUT_NAME to "Common" so the file becomes libCommon.a
set_target_properties(libCommon PROPERTIES
  OUTPUT_NAME "Common"            # produces libCommon.a / Common.lib
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)
