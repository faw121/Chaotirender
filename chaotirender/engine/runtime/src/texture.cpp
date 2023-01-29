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

        // debug
        // uint32_t max_index = width * height * 4 - 1;

        // if ((index + 3) > max_index)
        // {
        //     std::cout << "max index:" << max_index << std::endl;
        //     std::cout << "current max index:" << index + 3 << std::endl;

        //     std::cout << "u:" << u << " v:" << v <<std::endl;
        //     std::cout << "x:" << x << " y:" << y <<std::endl;
        // }
        
        // uint8_t  t = texels[max_index];
        uint8_t r0, g0, b0, a0;
        r0 = texels[0];
        g0 = texels[1];
        b0 = texels[2];
        a0 = texels[3];

        // std::cout << "r0, g0, b0, a0: " << r0 << ',' << g0 << ',' << b0 << ',' << a0 << std::endl;

        uint8_t  r = texels[index];
        uint8_t  g = texels[index + 1];
        uint8_t  b = texels[index + 2];
        uint8_t  a = texels[index + 3];

        // if (u < 0.2f && u > 0.1f && v < 0.2f && v > 0.1f)
            // std::cout << "r, g, b, a: " << r << ',' << g << ',' << b << ',' << a << std::endl;
        return glm::vec4(r, g, b, a);
    }
}