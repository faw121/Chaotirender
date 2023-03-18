#pragma once

#include <runtime/pipeline/data_type/texture.h>

#include <memory>

#include <string>

namespace Chaotirender
{
    class RenderResource
    {
    public:
        std::shared_ptr<Texture> loadTexture(std::string file);
        uint8_t* loadRawTexture(std::string file, int& w, int&h, int& n);
    };
}