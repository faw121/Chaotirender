#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <any>
#include <map>

namespace Chaotirender
{
    struct Fragment
    {   
        glm::i32vec2 screen_coord;
        float        depth;
        glm::vec3    normal;
        glm::vec2    uv;
        glm::vec3    world_position;
        glm::vec3    light_position; // light space position
        glm::vec3    color;

        // std::map<std::string, std::any> varyings;
    };
}


