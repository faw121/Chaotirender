#pragma once

#include <runtime/pixel_shader.h>

namespace Chaotirender
{
    
    class PixelProcessor
    {
    public:
        void processPixel();

        PixelShader* pixel_shader;
    };
}