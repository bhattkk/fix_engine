file(GLOB SRC_FILES "*.cpp")

add_library(Parser ${SRC_FILES})

target_include_directories(Parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Parser
  PUBLIC
    Interfaces
    Common
)

set_target_properties(Parser PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)
