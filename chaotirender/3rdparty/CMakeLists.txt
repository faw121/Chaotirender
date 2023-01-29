add_subdirectory(glm)
add_subdirectory(tinyobjloader)

if(NOT TARGET stb)
    include(stb.cmake)
endif()