#include <runtime/render_pipeline.h>

#include <runtime/render_pipeline_global_context.h>
#include <runtime/rasterization.h>

#include <runtime/asset_manager.h>
#include <runtime/simple_vertex_shader.h>
#include <runtime/geometry_processor.h>

#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtx/string_cast.hpp>

#include <iostream>


namespace Chaotirender
{   
    // TODO: 
    // 1. refractor pipeline as a global context, with ptr to geometry, rasterization, fragment shader
    // 2. how to store fragment buffer (vector of vector is bad)
    // 3. absract class for shaders
    // 3. learn smart pointer 
    // test examples
    void drawWireFrame()
    {
        int num_faces = g_pipeline_global_context.index_buffer.size() / 3;
        for (int f = 0; f < num_faces; f++)
        {   
            int index = f * 3;

            for (int i = 0; i < 3; i++)
            {
                glm::vec3 p0 = g_pipeline_global_context.vertex_buffer[g_pipeline_global_context.index_buffer[index + i]].position;
                glm::vec3 p1 = g_pipeline_global_context.vertex_buffer[g_pipeline_global_context.index_buffer[index + (i + 1) % 3]].position;

                rasterizeLine(p0.x, p0.y, p1.x, p1.y);
            }
        }
    }

    void drawTriangle()
    {   
        int num_faces = g_pipeline_global_context.geometry_index_buffer.size() / 3;

        for (int f = 0; f < num_faces; f++)
        {   int index = f * 3;
            glm::vec4 p0 = g_pipeline_global_context.geometry_vertex_buffer[g_pipeline_global_context.geometry_index_buffer[index + 0]].position_homo;
            glm::vec4 p1 = g_pipeline_global_context.geometry_vertex_buffer[g_pipeline_global_context.geometry_index_buffer[index + 1]].position_homo;
            glm::vec4 p2 = g_pipeline_global_context.geometry_vertex_buffer[g_pipeline_global_context.geometry_index_buffer[index + 2]].position_homo;

            p0 = p0 / p0.w;
            p1 = p1 / p1.w;
            p2 = p2 / p2.w;

            // debug
            std::cout << "screen:" << std::endl;
            std::cout << glm::to_string(p0) << std::endl;
            std::cout << glm::to_string(p1) << std::endl;
            std::cout << glm::to_string(p2) << std::endl;

            triangleSetup(p0, p1, p2);

            // bounding box of triangle
            TriangleBoundingBox bounding_box;
            triangleBoundingBox(p0, p1, p2, bounding_box);

            // triangle traversal
            for (int i = bounding_box.xmin; i <= bounding_box.xmax; i++)
            {
                for (int j = bounding_box.ymin; j <= bounding_box.ymax; j++)
                {
                    if (insideTriangle(i + 0.5f, j + 0.5f))
                    {
                        g_pipeline_global_context.color_buffer.set(i, j, g_pipeline_global_context.draw_color);
                    }
                }
            }
        }
    }

    void testPipeline()
    {
        // Vertex v1(-0.5, 0.5, 1);
        // Vertex v2(0, 0.5, -2);
        // Vertex v3(1.5, -0.5, -3);

        Vertex v1(-0.5, 0, 1);
        Vertex v2(0, -0.5, -3);
        Vertex v3(0.5, 0.5, -5);

        g_pipeline_global_context.vertex_buffer.push_back(v1);
        g_pipeline_global_context.vertex_buffer.push_back(v2);
        g_pipeline_global_context.vertex_buffer.push_back(v3);

        g_pipeline_global_context.index_buffer.push_back(0);
        g_pipeline_global_context.index_buffer.push_back(1);
        g_pipeline_global_context.index_buffer.push_back(2);

        SimpleVertexShader simple_vertex_shader;
        simple_vertex_shader.projection_matrix = glm::perspective(glm::half_pi<float>(), 1.f, 1.f, 4.f);

        GeometryProcessor geometry_processor;
        geometry_processor.initialize(g_pipeline_global_context.screen_width, g_pipeline_global_context.screen_height);
        geometry_processor.vertex_shader = &simple_vertex_shader;

        geometry_processor.processGeometry();

        drawTriangle();

        g_pipeline_global_context.color_buffer.write_tga_file("output.tga");       
    }

    void runPipeline()
    {
        loadAsset("asset/african_head.obj");

        bindPipelineBuffer();

        for (int i = 0; i < g_pipeline_global_context.vertex_buffer.size(); i++)
        {
            g_pipeline_global_context.vertex_buffer[i].position.x = \
                (g_pipeline_global_context.vertex_buffer[i].position.x + 1) * g_pipeline_global_context.screen_width / 2;
            g_pipeline_global_context.vertex_buffer[i].position.y = \
                (g_pipeline_global_context.vertex_buffer[i].position.y + 1) * g_pipeline_global_context.screen_height / 2;
        }

        // vertex_buffer.push_back(Vertex(0, screen_height / 2, 0, 0, 0, 0, 0, 0));
        // vertex_buffer.push_back(Vertex(screen_width / 2, screen_height / 2, 0, 0, 0, 0, 0, 0));
        // vertex_buffer.push_back(Vertex(screen_width / 3, 0, 0, 0, 0, 0, 0, 0));

        drawWireFrame();
        // drawTriangle();
        
        g_pipeline_global_context.color_buffer.write_tga_file("output.tga");
    }

}