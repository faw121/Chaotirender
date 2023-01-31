set(TARGET_NAME engineEditor)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp)

add_executable(${TARGET_NAME} ${srcs})

add_custom_target(remove_asset
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CHAOTIRENDER_BINARY_ROOT_DIR}/asset)
add_custom_target(copy_asset
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSET_DIR} ${CHAOTIRENDER_BINARY_ROOT_DIR}/asset DEPENDS remove_asset)

target_include_directories(${TARGET_NAME} PUBLIC include)

target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC imgui)
target_link_libraries(${TARGET_NAME} PUBLIC engineRuntime)