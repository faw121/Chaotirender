#pragma once

#include <runtime/fragment.h>

namespace Chaotirender
{
    class PixelShader
    {
    public:
        virtual void shadePixel(Fragment fragment) = 0;
    };
}