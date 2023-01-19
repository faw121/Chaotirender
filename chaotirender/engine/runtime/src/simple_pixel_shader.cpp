#include <runtime/simple_pixel_shader.h>

#include <glm/geometric.hpp>

namespace Chaotirender
{
    void SimplePixelShader::shadePixel(Fragment fragment)
    {
        float k = glm::dot(glm::normalize(light.position - fragment.world_position), glm::normalize(fragment.normal));
        k = k < 0 ? 0 : k;
        fragment.color = light.color * k; 
    }
} 