#include <runtime/camera.h>

namespace Chaotirender
{
    void Camera::move(glm::vec3 delta) { m_position += delta; }

    void Camera::rotate(float pitch, float yaw)
    {
        pitch = glm::radians(pitch);
        yaw = glm::radians(yaw);

        float dot = glm::dot(m_up_axis, forward());
        if ((dot < -0.99f && pitch > 0.0f) || // angle nearing 180 degrees
            (dot > 0.99f && pitch < 0.0f))    // angle nearing 0 degrees
            pitch = 0.0f;

        glm::quat q_pitch, q_yaw;
        q_pitch = glm::angleAxis(pitch, Z);
        q_yaw = glm::angleAxis(yaw, Y);

        m_rotation = q_pitch * m_rotation * q_yaw;
        m_invRotation = glm::conjugate(m_rotation);
    }

    void Camera::zoom(float offset)
    {
        m_fov = glm::clamp(m_fov - glm::radians(offset), MIN_FOV, MAX_FOV);
    }

    void Camera::lookAt(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
    {
        m_position = position;

        glm::vec3 forward = glm::normalize(target - position);
        m_rotation = glm::rotation(forward, X);

        glm::vec3 right = glm::normalize(glm::cross(forward, up));
        glm::vec3 orth_up = glm::cross(right, forward);

        glm::quat up_rotation = glm::rotation((m_rotation * orth_up), Y);

        m_rotation = up_rotation * m_rotation;
        m_invRotation = glm::conjugate(m_rotation);
    }

    glm::mat4x4 Camera::getViewMatrix() const
    {
        return glm::lookAt(m_position, m_position + forward(), up());
    }

    glm::mat4x4 Camera::getProjectionMatrix() const
    {
        return glm::perspective(m_fov, m_aspect, m_znear, m_zfar);
    }
}