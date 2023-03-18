#pragma once

#include <runtime/shader/base/pixel_shader.h>
#include <runtime/render/light.h>

namespace Chaotirender
{   
    class TexturePixelShader: public PixelShader
    {
    public:
        virtual void shadePixel(Fragment& fragment) override;

        Light light;

        glm::vec3 camera_position;
    };
}