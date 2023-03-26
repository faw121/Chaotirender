#include <runtime/render/render_system.h>

#include <runtime/shader/simple_vertex_shader.h>
#include <runtime/shader/phong_pixel_shader.h>
#include <runtime/global/engine_global_context.h>
#include <runtime/pipeline/render_pipeline.h>

#include <glm/mat4x4.hpp>

#include <runtime/tick_tock.h>
#include <runtime/debug.h>

namespace Chaotirender
{
    void RenderSystem::init()
    {
        // camera setup
        m_camera.setFov(glm::half_pi<float>() / 2);
        m_camera.lookAt(glm::vec3(0, 0, 2.8f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        // shaders
        m_vertex_shader_map["simple"] = std::make_shared<SimpleVertexShader>();
        m_pixel_shader_map["phong"] = std::make_shared<PhongPixelShader>();

        // scene
        m_render_scene.init();
    }

    void RenderSystem::swapRenderData()
    {   
        m_render_scene.m_render_mesh_list.clear();

        // light
        m_render_scene.m_ambient_light.m_intensity = g_engine_global_context.m_scene_manager->m_ambient_intensity;

        // calculate point light position
        float pitch = g_engine_global_context.m_scene_manager->m_point_pitch;
        float yaw = g_engine_global_context.m_scene_manager->m_point_yaw;
        float radius = g_engine_global_context.m_scene_manager->m_point_radius;

        float xy_radius = radius * glm::sin(glm::radians(pitch));
        glm::vec3 position(xy_radius * glm::sin(glm::radians(yaw)), radius * glm::cos(glm::radians(pitch)), xy_radius * glm::cos(glm::radians(yaw)));
        m_render_scene.m_point_light.m_position = position;
        m_render_scene.m_point_light.m_intensity = g_engine_global_context.m_scene_manager->m_point_intensity;

        // fetch object instances that need to draw
        for (auto& obj: g_engine_global_context.m_scene_manager->m_object_instance_list)
        {   
            if (obj.m_draw)
            {
                // model matrix
                // TODO: rotate
                glm::mat4x4 model_mat(1.f);
                model_mat = glm::translate(model_mat, obj.m_transform.translation);
                model_mat = glm::scale(model_mat, obj.m_transform.scale);

                for (auto& mesh: obj.m_sub_mesh)
                {
                    RenderMesh render_mesh;

                    render_mesh.m_model_mat = model_mat;

                    render_mesh.m_index_buffer_id = mesh.m_mesh_id.m_index_buffer_id;
                    render_mesh.m_vertex_buffer_id = mesh.m_mesh_id.m_vertex_buffer_id;

                    render_mesh.m_material_type = mesh.m_sub_material.m_material_type;
                    render_mesh.m_primitive_type = obj.m_primitive_type;
                    render_mesh.m_use_tex = mesh.m_sub_material.m_use_tex;
                    render_mesh.m_phong_material = mesh.m_sub_material.m_phong_material;
                    render_mesh.m_material_tex_id = mesh.m_tex_id;

                    m_render_scene.m_render_mesh_list.push_back(render_mesh);
                }
            }
        }
    }

    void RenderSystem::drawOneFrame(int& scene_w, int& scene_h, const uint8_t*& data)
    {
        g_render_pipeline.frame_buffer.clear();

        // g_render_pipeline.render_config.enable_parallel = false;
        // g_render_pipeline.render_config.rasterize_config.back_face_culling = false;
        // g_render_pipeline.render_config.rasterize_config.primitive = PrimitiveType::line;
        // g_render_pipeline.render_config.rasterize_config.line_color = Color(255, 255, 255);
        g_render_pipeline.render_config = m_render_config;

        drawCameraPass();        

        scene_w = g_render_pipeline.frame_buffer.getWidth();
        scene_h = g_render_pipeline.frame_buffer.getHeight();
        data = (const uint8_t*) g_render_pipeline.frame_buffer.getColorBuffer();
    }

    void RenderSystem::render()
    {   
        g_render_pipeline.frame_buffer.clear();

        drawCameraPass();
    }

    void RenderSystem::drawCameraPass()
    {   
        // TICK(prepare)
        // bind vertex shader
        std::shared_ptr<SimpleVertexShader> simple_vertex_shader = std::dynamic_pointer_cast<SimpleVertexShader>(m_vertex_shader_map["simple"]);
        g_render_pipeline.setVertexShader(simple_vertex_shader);

        // update view and projection matrix
        simple_vertex_shader->view_matrix = m_camera.getViewMatrix();
        simple_vertex_shader->projection_matrix = m_camera.getProjectionMatrix();

        // use phong shader
        std::shared_ptr<PhongPixelShader> phong_pixel_shader = std::dynamic_pointer_cast<PhongPixelShader>(m_pixel_shader_map["phong"]);
        g_render_pipeline.setPixelShader(phong_pixel_shader);

        // camera position
        phong_pixel_shader->m_camera_position = m_camera.position();

        // light
        phong_pixel_shader->m_ambient_intensity = m_render_scene.m_ambient_light.m_intensity;
        phong_pixel_shader->m_directional_light = m_render_scene.m_directional_light;
        phong_pixel_shader->m_point_light = m_render_scene.m_point_light;

        // TOCK(prepare)
        for (auto& mesh: m_render_scene.m_render_mesh_list)
        {   
            // model matrix
            simple_vertex_shader->model_matrix = mesh.m_model_mat;

            // TICK(attributes)
            // bind mesh data
            g_render_pipeline.bindIndexBuffer(mesh.m_index_buffer_id);
            g_render_pipeline.bindVertexBuffer(mesh.m_vertex_buffer_id);
            
            // TODO: support more material
            phong_pixel_shader->m_ka = mesh.m_phong_material.ka;
            phong_pixel_shader->m_kd = mesh.m_phong_material.kd;
            phong_pixel_shader->m_ks = mesh.m_phong_material.ks;
            phong_pixel_shader->m_shininess = mesh.m_phong_material.shininess;

            phong_pixel_shader->m_use_diffuse_texture = false;
            // use tex
            if (mesh.m_use_tex)
            {   
                phong_pixel_shader->m_use_diffuse_texture = true;
                g_render_pipeline.bindPixelShaderTexture(mesh.m_material_tex_id.m_base_color_tex_id, "diffuse_texture", SampleType::BILINEAR);
            }

            // g_render_pipeline.bindPixelShaderTexture(mesh.m_material_tex_id.m_base_color_tex_id, "diffuse_texture", SampleType::BILINEAR);
            // TOCK(attributes)

            // TICK(draw)
            // g_render_pipeline.render_config.enable_parallel = false;
            // g_render_pipeline.render_config.rasterize_config.primitive = PrimitiveType::line;
            g_render_pipeline.draw();
            // TOCK(draw)
        }
    }
}