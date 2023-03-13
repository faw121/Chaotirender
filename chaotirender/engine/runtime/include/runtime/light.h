#pragma once

#include <glm/vec3.hpp>

namespace Chaotirender
{
    struct Light
    {   
        glm::vec3 up;
        glm::vec3 position;
        glm::vec3 intensity;
    };
}