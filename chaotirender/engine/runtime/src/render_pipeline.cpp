#include <runtime/render_pipeline.h>

#include <runtime/render_pipeline_global_context.h>

#include <runtime/asset_manager.h>
#include <runtime/render_resource.h>

#include <runtime/simple_vertex_shader.h>
#include <runtime/simple_pixel_shader.h>
#include <runtime/texture_pixel_shader.h>

#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <chrono>

#include <runtime/tick_tock.h>

#include <runtime/parallel.h>

namespace Chaotirender
{   
    RenderPipeline::RenderPipeline(int w, int h): frame_buffer(w, h) {}

    buffer_id RenderPipeline::addVertexBuffer(const std::vector<Vertex>& data) { return m_vertex_buffer_list.add(data); }
    buffer_id RenderPipeline::addVertexBuffer(std::unique_ptr<std::vector<Vertex>>&& data) {  return m_vertex_buffer_list.add(std::move(data)); }

    buffer_id RenderPipeline::addIndexBuffer(const std::vector<index_t>& data) { return m_index_buffer_list.add(data); }
    buffer_id RenderPipeline::addIndexBuffer(std::unique_ptr<std::vector<index_t>>&& data) { return m_index_buffer_list.add(std::move(data)); }

    buffer_id RenderPipeline::addTexture(std::unique_ptr<Texture>&& texture) { return m_texture_buffer_list.add(std::move(texture)); }
    buffer_id RenderPipeline::addTexture(int w, int h, int channels, uint8_t* texels) { return m_texture_buffer_list.add(std::make_unique<Texture>(w, h, channels, texels)); }

    void RenderPipeline::bindVertexBuffer(buffer_id id)
    {
        m_vertex_buffer = m_vertex_buffer_list.get(id);
    }

    void RenderPipeline::bindIndexBuffer(buffer_id id)
    {
        m_index_buffer = m_index_buffer_list.get(id);
    }

    void RenderPipeline::removeVertexBuffer(buffer_id id)
    {
        if (m_vertex_buffer == m_vertex_buffer_list.get(id))
            m_vertex_buffer = nullptr;
        m_vertex_buffer_list.remove(id);
    }

    void RenderPipeline::removeIndexBuffer(buffer_id id)
    {
        if (m_index_buffer == m_index_buffer_list.get(id))
            m_index_buffer = nullptr;
        m_index_buffer_list.remove(id);
    }

    void RenderPipeline::clearVertexBuffer()
    {
        m_vertex_buffer = nullptr;
        m_vertex_buffer_list.clear();
    }

    void RenderPipeline::clearIndexBuffer()
    {
        m_index_buffer = nullptr;
        m_index_buffer_list.clear();
    }

    void RenderPipeline::setVertexShader(std::shared_ptr<VertexShader> shader) { m_vertex_shader = shader; }

    void RenderPipeline::setPixelShader(std::shared_ptr<PixelShader> shader) { m_pixel_shader = shader; }

    void RenderPipeline::bindVertexShaderTexture(buffer_id id, std::string name) { m_vertex_shader->texture_list[name] = m_texture_buffer_list.get(id); }

    void RenderPipeline::bindPixelShaderTexture(buffer_id id, std::string name) { m_pixel_shader->texture_list[name] = m_texture_buffer_list.get(id); }

    void RenderPipeline::removeTexture(buffer_id id)
    {
        auto ptr = m_texture_buffer_list.get(id);

        for (auto& pair: m_vertex_shader->texture_list)
            if (pair.second == ptr)
                pair.second = nullptr;

        for (auto& pair: m_pixel_shader->texture_list)
            if (pair.second == ptr)
                pair.second = nullptr;  
        m_texture_buffer_list.remove(id);
    }

    void RenderPipeline::clearTexture()
    {
        for (auto& pair: m_vertex_shader->texture_list)
            pair.second = nullptr;

        for (auto& pair: m_pixel_shader->texture_list)
            pair.second = nullptr;
        m_texture_buffer_list.clear();
    }
    
    void runPipelineParallel()
    {
        loadAsset("asset/spot/spot_triangulated_good.obj");

        // load texture
        RenderResource render_resource;
        
        std::shared_ptr<Texture> texture = render_resource.loadTexture("asset/spot/spot_texture.png");

        bindPipelineBuffer();

        SimpleVertexShader simple_vertex_shader;
        SimplePixelShader  simple_pixel_shader;
        TexturePixelShader texture_pixel_shader;

        // simple_vertex_shader.model_matrix = glm::translate(simple_vertex_shader.model_matrix, glm::vec3(0, 0, -3));
        glm::mat4 model_mat(1), view_mat, projection_mat;

        model_mat = glm::rotate(model_mat, 3 * glm::pi<float>() / 4, glm::vec3(0, 1, 0));
        // model_mat = glm::rotate(model_mat, glm::pi<float>() / 4, glm::vec3(1, 0, 0));
        view_mat = glm::lookAt(glm::vec3(0, 0, 2.8f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        projection_mat = glm::perspective(glm::half_pi<float>() / 2, 1.f, 0.1f, 100.f); // glm::perspective(glm::half_pi<float>(), 1.f, 0.1f, 10.f);
        // projection_mat = glm::ortho(-2.f, 2.f, -2.f, 2.f, 0.1f, 100.f);

        simple_vertex_shader.model_matrix = model_mat;
        simple_vertex_shader.view_matrix = view_mat;
        simple_vertex_shader.projection_matrix = projection_mat;

        Light light;
        light.position = glm::vec3(0, 0, 12);
        light.color = glm::vec3(255, 255, 255);

        texture_pixel_shader.light = light;
        simple_pixel_shader.light = light;

        g_pipeline_global_context.setVertexShader(&simple_vertex_shader);
        // g_pipeline_global_context.setPixelShader(&simple_pixel_shader);
        g_pipeline_global_context.setPixelShader(&texture_pixel_shader);

        // texture->sample_type = SampleType::BILINEAR;
        g_pipeline_global_context.bindTexture(texture);

        std::cout << "faces: " << g_pipeline_global_context.index_buffer.size() / 3 << std::endl;

        TICK(time);
        parallel_render();
        TOCK(time);

        g_pipeline_global_context.color_buffer.write_tga_file("spotp.tga");
        std::cout << "done" << std::endl;
    }

    void runPipeline()
    {
        loadAsset("asset/spot/spot_triangulated_good.obj");

        // load texture
        RenderResource render_resource;
        
        std::shared_ptr<Texture> texture = render_resource.loadTexture("asset/spot/spot_texture.png");

        bindPipelineBuffer();

        SimpleVertexShader simple_vertex_shader;
        SimplePixelShader  simple_pixel_shader;
        TexturePixelShader texture_pixel_shader;

        // simple_vertex_shader.model_matrix = glm::translate(simple_vertex_shader.model_matrix, glm::vec3(0, 0, -3));
        glm::mat4 model_mat(1), view_mat, projection_mat;

        model_mat = glm::rotate(model_mat, 3 * glm::pi<float>() / 4, glm::vec3(0, 1, 0));
        // model_mat = glm::rotate(model_mat, glm::pi<float>() / 4, glm::vec3(1, 0, 0));
        view_mat = glm::lookAt(glm::vec3(0, 0, 2.8f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        projection_mat = glm::perspective(glm::half_pi<float>() / 2, 1.f, 0.1f, 100.f); // glm::perspective(glm::half_pi<float>(), 1.f, 0.1f, 10.f);
        // projection_mat = glm::ortho(-2.f, 2.f, -2.f, 2.f, 0.1f, 100.f);

        simple_vertex_shader.model_matrix = model_mat;
        simple_vertex_shader.view_matrix = view_mat;
        simple_vertex_shader.projection_matrix = projection_mat;

        Light light;
        light.position = glm::vec3(0, 0, 12);
        light.color = glm::vec3(255, 255, 255);

        texture_pixel_shader.light = light;
        simple_pixel_shader.light = light;

        g_pipeline_global_context.setVertexShader(&simple_vertex_shader);
        // g_pipeline_global_context.setPixelShader(&simple_pixel_shader);
        g_pipeline_global_context.setPixelShader(&texture_pixel_shader);

        // texture->sample_type = SampleType::BILINEAR;
        g_pipeline_global_context.bindTexture(texture);

        TICK(runtime)

        g_pipeline_global_context.runPipeline();

        TOCK(runtime)

        g_pipeline_global_context.color_buffer.write_tga_file("spot.tga");
    }
}