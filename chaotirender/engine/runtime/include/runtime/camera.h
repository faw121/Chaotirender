#pragma once

#include <glm/vec3.hpp>
#include <glm/glm.hpp>

namespace Chaotirender
{
    class Camera
    {
    private:
        glm::vec3 m_position;
        glm::vec3 m_up;
        float m_znear;
        float m_zfar;
        float m_fov;
        float m_aspect;
        glm::lowp_quat
    };
} // namespace Chaotirender
