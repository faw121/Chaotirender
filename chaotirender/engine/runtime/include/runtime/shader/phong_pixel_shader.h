#pragma once

#include <runtime/shader/base/pixel_shader.h>
#include <runtime/render/light.h>

namespace Chaotirender
{   
    // diffuse texture name: "diffuse_texture"
    class PhongPixelShader: public PixelShader
    {
    public:
        virtual void shadePixel(Fragment& fragment) override;

        Light m_light;

        DirectionalLight m_directional_light;

        PointLight m_point_light;

        glm::vec3 m_ambient_intensity {50.f, 50.f, 50.f};
        
        glm::vec3 m_camera_position;

        bool m_use_diffuse_texture;

        glm::vec3 m_ka {0.4f, 0.4f, 0.4f};
        glm::vec3 m_kd {0.7f, 0.7f, 0.7f};
        glm::vec3 m_ks {0.3f, 0.3f, 0.3f};
        float m_shininess {50.f};
    
    private:
        glm::vec3 phongShading(Fragment& fragment);
    };
}