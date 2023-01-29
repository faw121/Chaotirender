#include <runtime/texture.h>

#include <cassert>

#include <runtime/debug.h>

namespace Chaotirender
{   
    glm::vec4 Texture::sample(float u, float v)
    {
        return sampleNearest(u, v);
    }

    glm::vec4 Texture::sampleNearest(float u, float v)
    {
        assert(texels != nullptr && "no texture loaded!");
        
        // debug: handle uv out of range
        u = u < 0 ? 0 : u;
        v = v < 0 ? 0 : v;

        u = u > 1 ? 1 : u;
        v = v > 1 ? 1 : v;

        uint32_t x = static_cast<uint32_t>(u * (width - 1));
        uint32_t y = static_cast<uint32_t>((1 - v) * (height - 1));
        
        uint32_t index = (y * width + x) * 4;

        uint8_t r0, g0, b0, a0;
        r0 = texels[0];
        g0 = texels[1];
        b0 = texels[2];
        a0 = texels[3];

        uint8_t  r = texels[index];
        uint8_t  g = texels[index + 1];
        uint8_t  b = texels[index + 2];
        uint8_t  a = texels[index + 3];

        return glm::vec4(r, g, b, a);
    }
}