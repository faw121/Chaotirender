#pragma once

#include <runtime/shader/base/pixel_shader.h>
#include <runtime/render/light.h>

namespace Chaotirender
{   
    class SimplePixelShader: public PixelShader
    {
    public:
        virtual void shadePixel(Fragment& fragment) override;

        Light light;
    };
}