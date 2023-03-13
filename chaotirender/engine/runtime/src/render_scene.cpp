#include <runtime/render_scene.h>

namespace Chaotirender
{   
    void RenderScene::init()
    {   
        // shaders
        m_simple_vertex_shader = std::make_shared<SimpleVertexShader>();
        m_phong_pixel_shader = std::make_shared<PhongPixelShader>();

        // camera setup
        m_camera.setFov(glm::half_pi<float>() / 2);
        m_camera.lookAt(glm::vec3(0, 0, 2.8f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        // light
        m_light.position = glm::vec3(0, 100, 0);
        m_light.intensity = glm::vec3(100, 100, 100);
    }

     
    void RenderScene::draw()
    {
        // bind vertex shader
        g_render_pipeline.setVertexShader(m_simple_vertex_shader);

        drawCameraPass();   
    }

    void RenderScene::drawCameraPass()
    {   
        // update view and projetion matrix
        m_simple_vertex_shader->view_matrix = m_camera.getViewMatrix();
        m_simple_vertex_shader->projection_matrix = m_camera.getProjectionMatrix();

        // update camera and light position
        m_phong_pixel_shader->m_camera_position = m_camera.position();
        m_phong_pixel_shader->m_light =m_light;

         // bind shader
        g_render_pipeline.setPixelShader(m_phong_pixel_shader);

        // draw each object, use phong shading
        for (auto& object: m_object_list)
        {   
            // update model mat
            glm::mat4x4 model_mat(1);
            
            // TODO: rotate
            glm::translate(model_mat, object.transform.translation);
            glm::scale(model_mat, object.transform.scale);
            
            m_simple_vertex_shader->model_matrix = model_mat;

            // per mesh draw
            for (auto& mesh: object.mesh_list)
            {   
                // use texture?
                m_phong_pixel_shader->m_use_diffuse_texture = object.use_texture;

                // but if current mesh no texture
                if (mesh.material.diffuse_tex_id == -1)
                    m_phong_pixel_shader->m_use_diffuse_texture = false;

                // update shader attributes
                m_phong_pixel_shader->m_kd = mesh.material.kd;
                m_phong_pixel_shader->m_ks = mesh.material.ks;
                m_phong_pixel_shader->m_shininess = mesh.material.shininess;

                mesh.draw();
            }
        }
    }
} // namespace Chaotirender
