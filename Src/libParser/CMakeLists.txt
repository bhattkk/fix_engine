file(GLOB SRC_FILES "*.cpp")

add_library(libParser ${SRC_FILES})

# Require C++17 for this target
target_compile_features(libParser PUBLIC cxx_std_17)

# Link to header-only deps
target_link_libraries(libParser
  PUBLIC
    libCommon
    Interfaces
)

# If you want to avoid the double "lib" prefix on Unix (liblibParser.a),
# rename the OUTPUT_NAME to "Parser" so the file becomes libParser.a
set_target_properties(libParser PROPERTIES
  OUTPUT_NAME "Parser"            # produces libParser.a / Parser.lib
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)
