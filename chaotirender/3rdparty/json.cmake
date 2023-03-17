# file(GLOB json_sources CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/json/*.hpp")
# add_library(json INTERFACE ${json_sources})
add_library(json INTERFACE)
target_include_directories(json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/json)