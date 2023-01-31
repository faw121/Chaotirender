set(TARGET_NAME engineRuntime)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

add_executable(${TARGET_NAME} ${srcs})

add_custom_target(remove_asset
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CHAOTIRENDER_BINARY_ROOT_DIR}/asset)
add_custom_target(copy_asset
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSET_DIR} ${CHAOTIRENDER_BINARY_ROOT_DIR}/asset DEPENDS remove_asset)

add_dependencies(${TARGET_NAME} copy_asset)

target_include_directories(${TARGET_NAME} PUBLIC include)

target_link_libraries(${TARGET_NAME} PUBLIC glm)
target_link_libraries(${TARGET_NAME} PUBLIC tinyobjloader)
target_link_libraries(${TARGET_NAME} PRIVATE stb)
target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC imgui)