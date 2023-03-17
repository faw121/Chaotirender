set(TARGET_NAME engineRuntime)

file(GLOB_RECURSE headers CONFIGURE_DEPENDS include/*.h)
file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp)

set(TBB_DIR C:/Librarys/oneapi-tbb-2021.7.0-win/oneapi-tbb-2021.7.0/lib/cmake/tbb)
find_package(TBB REQUIRED)

# add_executable(${TARGET_NAME} ${srcs})
add_library(${TARGET_NAME} ${headers} ${srcs})

# add_custom_target(remove_asset
#     COMMAND ${CMAKE_COMMAND} -E remove_directory ${CHAOTIRENDER_BINARY_ROOT_DIR}/asset)
# add_custom_target(copy_asset
#     COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSET_DIR} ${CHAOTIRENDER_BINARY_ROOT_DIR}/asset DEPENDS remove_asset)

# add_dependencies(${TARGET_NAME} copy_asset)

target_include_directories(${TARGET_NAME} PUBLIC include)

target_link_libraries(${TARGET_NAME} PUBLIC glm)
target_link_libraries(${TARGET_NAME} PUBLIC tinyobjloader)
target_link_libraries(${TARGET_NAME} PRIVATE stb)
target_link_libraries(${TARGET_NAME} PUBLIC  json)
target_link_libraries(${TARGET_NAME} PUBLIC TBB::tbb)