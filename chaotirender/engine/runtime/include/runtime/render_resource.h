#pragma once

#include <runtime/texture.h>

#include <memory>

namespace Chaotirender
{
    class RenderResource
    {
    public:
        std::shared_ptr<Texture> loadTexture(std::string file);
    };
}