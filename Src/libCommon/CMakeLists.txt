file(GLOB SRC_FILES "*.cpp")

add_library(Common ${SRC_FILES})

# Making this public so that any target linking to Common also gets the include dirs
target_include_directories(Common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Common
  PUBLIC
    Interfaces
)

set_target_properties(Common PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)
