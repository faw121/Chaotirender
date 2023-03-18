#pragma once

#include <runtime/pipeline/data_type/fragment.h>
#include <runtime/pipeline/data_type/texture.h>

#include <memory>
#include <map>
#include <string>

namespace Chaotirender
{
    class PixelShader
    {
    public:
        virtual ~PixelShader() {};

        virtual void shadePixel(Fragment& fragment) = 0;

        std::shared_ptr<TextureBase> texture {nullptr};

        // multiple texture binding
        std::map<std::string, TextureBase*> texture_list;
    };
}