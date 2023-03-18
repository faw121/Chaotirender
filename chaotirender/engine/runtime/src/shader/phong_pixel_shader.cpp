#include <runtime/shader/phong_pixel_shader.h>

#include <glm/geometric.hpp>

namespace Chaotirender
{   
    glm::vec3 PhongPixelShader::phongShading(Fragment& fragment)
    {
        glm::vec3 kd = m_kd;
        if (m_use_diffuse_texture)
            kd = texture_list["diffuse_texture"]->sample(fragment.uv.x, fragment.uv.y) / 255.f;

        glm::vec3 ambient = ambient_intensity * kd;

        glm::vec3 frag_position = fragment.world_position;
        glm::vec3 normal = glm::normalize(fragment.normal);

        glm::vec3 light_direction = glm::normalize(m_light.position - frag_position);
        glm::vec3 view_direction = glm::normalize(m_camera_position - frag_position);
        float dot = glm::max(glm::dot(light_direction, normal), 0.f);

        glm::vec3 diffuse = kd * m_light.intensity * dot;

        glm::vec3 half = glm::normalize(light_direction + view_direction);

        glm::vec3 specular(0.f, 0.f, 0.f);

        if (m_ks != glm::vec3(0.f, 0.f, 0.f))
            specular = m_ks * m_light.intensity * glm::pow(glm::max(glm::dot(half, normal), 0.f), m_shininess);

        return ambient + diffuse + specular;
    }

    void PhongPixelShader::shadePixel(Fragment& fragment)
    {   
        glm::vec3 color = phongShading(fragment);
        fragment.color = color;
    }
}