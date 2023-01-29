#pragma once

#include <runtime/fragment.h>
#include <runtime/texture.h>

#include <memory>

namespace Chaotirender
{
    class PixelShader
    {
    public:
        virtual void shadePixel(Fragment& fragment) = 0;

        std::shared_ptr<TextureBase> texture {nullptr};
    };
}