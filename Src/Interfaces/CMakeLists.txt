add_library(Interfaces INTERFACE)
# Tell consumers where to find this headers
target_include_directories(Interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
