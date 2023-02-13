#pragma once

#include <runtime/fragment.h>
#include <runtime/texture.h>

#include <memory>
#include <map>
#include <string>

namespace Chaotirender
{
    class PixelShader
    {
    public:
        virtual void shadePixel(Fragment& fragment) = 0;

        std::shared_ptr<TextureBase> texture {nullptr};

        // multiple texture binding
        std::map<std::string, TextureBase*> texture_list;
    };
}