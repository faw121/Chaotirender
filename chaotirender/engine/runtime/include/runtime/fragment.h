#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace Chaotirender
{
    struct Fragment
    {   
        glm::i32vec2 screen_coord;
        float        depth;
        glm::vec3    normal;
        glm::vec2    uv;
        glm::vec3    world_position;
        glm::vec3    color;
    };
}


