#pragma once

#include <runtime/texture.h>

#include <memory>

#include <string>

namespace Chaotirender
{
    class RenderResource
    {
    public:
        std::shared_ptr<Texture> loadTexture(std::string file);
    };
}