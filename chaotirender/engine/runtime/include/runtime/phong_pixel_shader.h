#pragma once

#include <runtime/pixel_shader.h>
#include <runtime/light.h>

namespace Chaotirender
{   
    // diffuse texture name: "diffuse_texture"
    class PhongPixelShader: public PixelShader
    {
    public:
        virtual void shadePixel(Fragment& fragment) override;

        Light m_light;
        glm::vec3 ambient_intensity {50.f, 50.f, 50.f};
        
        glm::vec3 m_camera_position;

        bool m_use_diffuse_texture;

        glm::vec3 m_kd {0.5f, 0.5f, 0.5f};
        glm::vec3 m_ks {0.8f, 0.8f, 0.8f};
        float m_shininess;
    
    private:
        glm::vec3 phongShading(Fragment& fragment);
    };
}