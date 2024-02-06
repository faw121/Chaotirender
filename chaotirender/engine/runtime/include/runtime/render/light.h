#pragma once

#include <glm/vec3.hpp>
#include <glm/gtx/transform.hpp>

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
        glm::vec3 m_position;
        glm::vec3 m_direction;
        glm::vec3 m_intensity {100.f, 100.f, 100.f};

        // glm::mat4 getViewMatrix() { return glm::lookAt(m_position, glm::vec3(0.f, 0.f, 0.f), m_up); }
        // glm::mat4 getProjectionMatrix() { return glm::perspective(glm::radians(60.f), 1.f, 0.1f, 500.f);}
    };

    struct PointLight
    {
        glm::vec3 m_position;
        glm::vec3 m_up;
        glm::vec3 m_right;
        glm::vec3 m_intensity {0.f, 0.f, 0.f};
    };
}