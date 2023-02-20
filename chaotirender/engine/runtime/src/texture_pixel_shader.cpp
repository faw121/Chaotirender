#include <runtime/texture_pixel_shader.h>

#include <glm/geometric.hpp>

namespace Chaotirender
{
    void TexturePixelShader::shadePixel(Fragment& fragment)
    {   
        // glm::vec3 tex_color = texture->sample(fragment.uv.x, fragment.uv.y);
        glm::vec3 tex_color = texture_list["texture"]->sample(fragment.uv.x, fragment.uv.y);
        // glm::vec3 kd = texture->sample(fragment.uv.x, fragment.uv.y) / 255.f;

        // float k = glm::dot(glm::normalize(light.position - fragment.world_position), glm::normalize(fragment.normal));
        // k = k < 0 ? 0 : k;

        // fragment.color = light.color * kd * k; 
        fragment.color = tex_color;
    }
}