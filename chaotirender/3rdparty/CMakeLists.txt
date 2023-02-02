if(NOT TARGET glfw)
    option(GLFW_BUILD_EXAMPLES "" OFF)
    option(GLFW_BUILD_TESTS "" OFF)
    option(GLFW_BUILD_DOCS "" OFF)
    option(GLFW_INSTALL "" OFF)
    add_subdirectory(glfw)
endif()

if(NOT TARGET imgui)
    include(imgui.cmake)
endif()

if(NOT TARGET tinyobjloader)
    add_subdirectory(tinyobjloader)
endif()

if(NOT TARGET glm)
    add_subdirectory(glm)
endif()

if(NOT TARGET stb)
    include(stb.cmake)
endif()

if(NOT TARGET glad)
    include(glad.cmake)
endif()