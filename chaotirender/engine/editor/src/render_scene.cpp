#include <editor/render_scene.h>

namespace Chaotirender
{   
    void RenderScene::init()
    {   
        // shaders
        simple_vertex_shader = std::make_shared<SimpleVertexShader>();
        texture_pixel_shader = std::make_shared<TexturePixelShader>();

        // camera setup
        camera.setFov(glm::half_pi<float>() / 2);
        camera.lookAt(glm::vec3(0, 0, 2.8f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    }

     
    void RenderScene::draw()
    {
        // update view and projetion matrix
        simple_vertex_shader->view_matrix = camera.getViewMatrix();
        simple_vertex_shader->projection_matrix = camera.getProjectionMatrix();

        // update camera position
        texture_pixel_shader->camera_position = camera.position();

        
    }

    void RenderScene::drawCameraPass()
    {
        // draw each object, use phong shading
        for (auto& object: object_list)
        {
            // update shader attributes
            
        }
    }
} // namespace Chaotirender
