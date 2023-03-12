#include <runtime/texture_pixel_shader.h>

#include <glm/geometric.hpp>

#include <runtime/debug.h>

#include <vector>

namespace Chaotirender
{
    void TexturePixelShader::shadePixel(Fragment& fragment)
    {   
        glm::vec3 tex_color = texture_list["texture"]->sample(fragment.uv.x, fragment.uv.y);
        
        // fragment.color = tex_color;
        
        glm::vec3 kd = tex_color / 255.f;
        glm::vec3 ka(0.5f, 0.5f, 0.5f);
        glm::vec3 ks(0.5f, 0.5f, 0.5f);
        float p = 50;

        std::vector<Light> lights(1);
        glm::vec3 ambient_intensity(100, 100, 100);

        lights[0].position = glm::vec3(0, 100, 0);
        lights[0].intensity = glm::vec3(100, 100, 100);

        // lights[1].position = glm::vec3(0, 200, 0);
        // lights[1].intensity = glm::vec3(50, 50, 50);
        
        glm::vec3 result(0, 0, 0);

        for (auto& light: lights)
        {   
            glm::vec3 ambient = ambient_intensity * ka * kd;
                
            glm::vec3 frag_position = fragment.world_position;
            glm::vec3 normal = glm::normalize(fragment.normal);

            glm::vec3 light_direction = glm::normalize(light.position - frag_position);
            glm::vec3 view_direction = glm::normalize(camera_position - frag_position);
            float dot = glm::max(glm::dot(light_direction, normal), 0.f);
            
            glm::vec3 diffuse = kd * light.intensity * dot;

            glm::vec3 half = glm::normalize(light_direction + view_direction);
            glm::vec3 specular = ks * light.intensity * glm::pow(glm::max(glm::dot(half, normal), 0.f), p);

            result += ambient;
            result += diffuse;
            result += specular;
        }
        fragment.color = result;
    }
}