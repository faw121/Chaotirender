#include <runtime/render/render_scene.h>

namespace Chaotirender
{   
    void RenderScene::init()
    {   
        // shaders
        m_simple_vertex_shader = std::make_shared<SimpleVertexShader>();
        m_phong_pixel_shader = std::make_shared<PhongPixelShader>();

        // camera setup
        m_camera.setFov(glm::half_pi<float>() / 2);
        m_camera.lookAt(glm::vec3(0, 0, 2.8f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        // light
        m_light.position = glm::vec3(0, 100, 0);
        m_light.intensity = glm::vec3(200, 200, 200);

        m_ambient_light.m_intensity = glm::vec3(50, 50, 50);

        m_point_light.m_position = glm::vec3(0, 100, 0);
        m_point_light.m_intensity = glm::vec3(200, 200, 200);

        m_directional_light.m_direction = glm::vec3(1.f, 1.f, -1.f);
        m_directional_light.m_intensity= glm::vec3(300, 300, 300);
    }
} // namespace Chaotirender
