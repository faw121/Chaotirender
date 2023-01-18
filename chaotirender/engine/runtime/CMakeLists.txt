file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

add_executable(engineRuntime ${srcs})

add_custom_target(remove_asset
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CHAOTIRENDER_BINARY_ROOT_DIR}/asset)
add_custom_target(copy_asset
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSET_DIR} ${CHAOTIRENDER_BINARY_ROOT_DIR}/asset DEPENDS remove_asset)

add_dependencies(engineRuntime copy_asset)

target_include_directories(engineRuntime PUBLIC include)

target_link_libraries(engineRuntime PUBLIC glm)
target_link_libraries(engineRuntime PUBLIC tinyobjloader)