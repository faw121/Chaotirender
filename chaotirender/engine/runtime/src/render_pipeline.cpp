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

namespace Chaotirender
{   

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

        auto t1 = std::chrono::high_resolution_clock::now();

        g_pipeline_global_context.runPipeline();

        auto t2 = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> time = t2 - t1;

        std::cout << "time spent: " << time.count() << "ms" << std::endl;

        g_pipeline_global_context.color_buffer.write_tga_file("spot.tga");
    }
}