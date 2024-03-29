set(TARGET_NAME engineEditor)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp)

add_executable(${TARGET_NAME} ${srcs})

# $<CONFIG>
add_custom_target(remove_asset
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CHAOTIRENDER_BINARY_ROOT_DIR}/asset VERBATIM)
add_custom_target(copy_asset
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSET_DIR} ${CHAOTIRENDER_BINARY_ROOT_DIR}/asset DEPENDS remove_asset VERBATIM)

add_dependencies(${TARGET_NAME} copy_asset)

add_custom_target(remove_resource
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CHAOTIRENDER_BINARY_ROOT_DIR}/resource VERBATIM)
add_custom_target(copy_resource
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCE_DIR} ${CHAOTIRENDER_BINARY_ROOT_DIR}/resource DEPENDS remove_resource VERBATIM)

add_dependencies(${TARGET_NAME} copy_resource)

# if(${CMAKE_BUILD_TYPE} MATCHES Release)
#     message(STATUS ${CMAKE_BUILD_TYPE})
#     message(STATUS $<TARGET_FILE_DIR:target>)
# endif()



# add_custom_target(remove_asset
#     COMMAND ${CMAKE_COMMAND} -E remove_directory ${CHAOTIRENDER_BINARY_ROOT_DIR}/asset)
# add_custom_target(copy_asset
#     COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSET_DIR} ${CHAOTIRENDER_BINARY_ROOT_DIR}/asset DEPENDS remove_asset)

target_include_directories(${TARGET_NAME} PUBLIC include)

# target_link_libraries(${TARGET_NAME} PUBLIC glm) # duplicate?
target_link_libraries(${TARGET_NAME} PUBLIC glad)
target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC imgui)
target_link_libraries(${TARGET_NAME} PUBLIC engineRuntime)