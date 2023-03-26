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

    struct AmbientLight
    {
        glm::vec3 m_intensity {50.f, 50.f, 50.f};
    };

    struct DirectionalLight
    {
        glm::vec3 m_direction;
        glm::vec3 m_intensity {100.f, 100.f, 100.f};
    };

    struct PointLight
    {
        glm::vec3 m_position;
        glm::vec3 m_intensity {0.f, 0.f, 0.f};
    };
}