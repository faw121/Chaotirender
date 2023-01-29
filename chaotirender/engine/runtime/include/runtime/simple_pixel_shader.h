#pragma once

#include <runtime/pixel_shader.h>
#include <runtime/light.h>

namespace Chaotirender
{   
    class SimplePixelShader: public PixelShader
    {
    public:
        virtual void shadePixel(Fragment& fragment) override;

        Light light;
    };
}