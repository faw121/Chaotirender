#include <runtime/render_resource.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <runtime/debug.h>

namespace Chaotirender
{
    std::shared_ptr<Texture> RenderResource::loadTexture(std::string file)
    {
        std::shared_ptr<Texture> texture = std::make_shared<Texture>();

        int w, h, n;
        texture->texels = static_cast<uint8_t*>(stbi_load(file.c_str(), &w, &h, &n, 4));

        if (!texture->texels)
            return nullptr;

        // debug
        std::cout << "w:" << w << " h:" << h << " n:" << n << std::endl;

        texture->width = w;
        texture->height = h;
        texture->channel = 4;
        texture->depth = 1;
        texture->mip_level = 1;

        return texture;
    }
}