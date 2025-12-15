add_library(Interfaces INTERFACE)

# Making this public so that any target linking to Interfaces also gets the include dirs
target_include_directories(Interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
