#include <runtime/render_resource.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <runtime/debug.h>

namespace Chaotirender
{
    std::shared_ptr<Texture> RenderResource::loadTexture(std::string file)
    {
        int w, h, n;
        auto texels = static_cast<uint8_t*>(stbi_load(file.c_str(), &w, &h, &n, 4));

        if (!texels)
            return nullptr;

        auto texture = std::make_shared<Texture>(w, h, 4, texels);

        texels = nullptr;

        // debug
        std::cout << "w:" << w << " h:" << h << " n:" << n << std::endl;

        return texture;
    }
}