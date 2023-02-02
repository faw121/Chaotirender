file(GLOB glad_sources CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/glad/src/*.c")

add_library(glad STATIC ${glad_sources})
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)