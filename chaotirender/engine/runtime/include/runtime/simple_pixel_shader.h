#pragma once

#include <runtime/pixel_shader.h>

namespace Chaotirender
{   
    struct Light
    {
        glm::vec3 position;
        glm::vec3 color;
    };

    class SimplePixelShader: public PixelShader
    {
    public:
        void shadePixel(Fragment fragment) override;

        Light light;
    };
}