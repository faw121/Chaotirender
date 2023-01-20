#include <runtime/render_pipeline.h>

#include <runtime/render_pipeline_global_context.h>
#include <runtime/rasterizer.h>

#include <runtime/asset_manager.h>
#include <runtime/simple_vertex_shader.h>
#include <runtime/simple_pixel_shader.h>
#include <runtime/geometry_processor.h>

#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtx/string_cast.hpp>

#include <iostream>


namespace Chaotirender
{   
    void runPipeline()
    {
        loadAsset("asset/african_head.obj");

        bindPipelineBuffer();

        // glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(-0.5,))) 
        // Vertex v1(-0.5, 0, 1);
        // Vertex v2(0, -0.5, -3);
        // Vertex v3(0.5, 0.5, -5);
        SimpleVertexShader simple_vertex_shader;

        SimplePixelShader simple_pixel_shader;

        Light light;
        light.position = glm::vec3(0, 0, 1);
        light.color = glm::vec3(255, 255, 255);
        simple_pixel_shader.light = light;

        simple_vertex_shader.model_matrix = glm::translate(simple_vertex_shader.model_matrix, glm::vec3(0, 0, -3));
        simple_vertex_shader.projection_matrix = glm::perspective(glm::half_pi<float>(), 1.f, 0.1f, 10.f);
        g_pipeline_global_context.setVertexShader(&simple_vertex_shader);
        

        g_pipeline_global_context.setPixelShader(&simple_pixel_shader);

        g_pipeline_global_context.runPipeline();

        g_pipeline_global_context.color_buffer.write_tga_file("output.tga");
    }
}