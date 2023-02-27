#pragma once

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Chaotirender
{
    class Camera
    {
    public:
        static const glm::vec3 X, Y, Z;
        static constexpr float MIN_FOV {0.1f};
        static constexpr float MAX_FOV {45.f};

        void move(glm::vec3 delta);
        void rotate(float pitch, float yaw); // pitch, yaw
        void zoom(float offset);
        void lookAt(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);

        void setAspect(float aspect) { m_aspect = aspect; }
        void setFov(float fov) { m_fov = fov; }

        glm::vec3 position() const { return m_position; }
        glm::quat rotation() const { return m_invRotation; }

        glm::vec3 forward() const { return (m_invRotation * X); }
        glm::vec3 up() const { return (m_invRotation * Y); }
        glm::vec3 right() const { return (m_invRotation * Z); }
        float getFov() const { return m_fov; }

        glm::mat4x4 getViewMatrix() const;
        glm::mat4x4 getProjectionMatrix() const;

    private:
        glm::vec3 m_position;
        glm::quat m_rotation {1.f, 0.f, 0.f, 0.f};
        glm::quat m_invRotation {1.f, 0.f, 0.f, 0.f};

        float m_znear {1.f};
        float m_zfar {1000.f};
        
        glm::vec3 m_up_axis {Y}; // up is Y

        float m_fov  {0.f};
        float m_aspect {1.f};
    };

    inline const glm::vec3 Camera::X = {1.f, 0.f, 0.f};
    inline const glm::vec3 Camera::Y = {0.f, 1.f, 0.f};
    inline const glm::vec3 Camera::Z = {0.f, 0.f, 1.f};

} // namespace Chaotirender
