#include <runtime/texture.h>

#include <cassert>

#include <runtime/debug.h>

namespace Chaotirender
{   
    glm::vec4 Texture::sample(float u, float v)
    {   
        assert(texels != nullptr && "no texture loaded!");

        // simply handle uv out of range
        u = u < 0 ? 0 : u;
        v = v < 0 ? 0 : v;

        u = u > 1 ? 1 : u;
        v = v > 1 ? 1 : v;
        
        if (sample_type == SampleType::NEAREST)
            return sampleNearest(u, v);
            
        else if (sample_type == SampleType::BILINEAR)
            return sampleBilinear(u, v);
    }

    glm::vec4 Texture::sampleNearest(float u, float v)
    {
        int x = static_cast<int>(u * (width - 1));
        int y = static_cast<int>((1 - v) * (height - 1));

        return getTexel(x, y);
    }

    glm::vec4 Texture::sampleBilinear(float u, float v)
    {
        // 4 nearest pixels
        float x = u * (width - 1);
        float y = (1 - v) * (height - 1);

        int i = static_cast<int>(x);
        int j = static_cast<int>(y);

        int sx = 1;
        int sy = 1;

        float tx = x - i - 0.5f;
        float ty = y - j - 0.5f;

        if (tx < 0)
        {
            sx = -1;
            tx = - tx;
        }
        if (ty < 0)
        {
            sy = -1;
            ty = - ty;
        }
        
        int x_near = i + sx;
        int y_near = j + sy;

        x_near = x_near < 0 ? 0 :x_near;
        x_near = x_near > width - 1 ? width - 1 : x_near;

        y_near = y_near < 0 ? 0 :y_near;
        y_near = y_near > height - 1 ? height - 1 : y_near;

        glm::vec4 c1 = getTexel(i, j);
        glm::vec4 c2 = getTexel(x_near, j);
        glm::vec4 c3 = getTexel(i, y_near);
        glm::vec4 c4 = getTexel(x_near, y_near);

        // interpolation
        glm::vec4 cx1 = (1.f - tx) * c1 + tx * c2;
        glm::vec4 cx2 = (1.f - tx) * c3 + tx * c4;

        glm::vec4 cy = (1.f - ty) * cx1 + ty * cx2;

        return cy;
    }

    glm::vec4 Texture::getTexel(int i, int j)
    {
        int index = (j * width + i) * 4;

        uint8_t  r = texels[index];
        uint8_t  g = texels[index + 1];
        uint8_t  b = texels[index + 2];
        uint8_t  a = texels[index + 3];

        return glm::vec4(r, g, b, a);
    }
}