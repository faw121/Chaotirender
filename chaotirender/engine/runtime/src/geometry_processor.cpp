#include <runtime/geometry_processor.h>

#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include <cassert>
#include <vector>
#include <algorithm>

namespace Chaotirender
{   
    void GeometryProcessor::initialize(int w, int h)
    {
        screen_mapping_matrix[0][0] = w / 2;
        screen_mapping_matrix[3][0] = w / 2;
        screen_mapping_matrix[1][1] = h / 2;
        screen_mapping_matrix[3][1] = h / 2;
        screen_mapping_matrix[2][2] = 1;
        screen_mapping_matrix[3][3] = 1;
    }

    void GeometryProcessor::processGeometry()
    {
        assert(vertex_shader != nullptr && "no vertex shader is set!");

        // for each face(triangle)
        int num_faces = g_pipeline_global_context.index_buffer.size() / 3;

        int in_index_base = 0;
        int out_index_base = 0;

        for (int f = 0; f < num_faces; f++)
        {   
            in_index_base = 3 * f;

            Vertex v0(g_pipeline_global_context.vertex_buffer[g_pipeline_global_context.index_buffer[in_index_base + 0]]);
            Vertex v1(g_pipeline_global_context.vertex_buffer[g_pipeline_global_context.index_buffer[in_index_base + 1]]);
            Vertex v2(g_pipeline_global_context.vertex_buffer[g_pipeline_global_context.index_buffer[in_index_base + 2]]);

            v0.position_homo = glm::vec4(v0.position, 1);
            v1.position_homo = glm::vec4(v1.position, 1);
            v2.position_homo = glm::vec4(v2.position, 1);

            // do vertex shading, transformed to [-1, 1]^3 (homogeneous space)
            vertex_shader->shadeVertex(v0);
            vertex_shader->shadeVertex(v1);
            vertex_shader->shadeVertex(v2);

            // debug
            std::cout << "after projection:" << std::endl;
            std::cout << glm::to_string(v0.position_homo) << std::endl;
            std::cout << glm::to_string(v1.position_homo) << std::endl;
            std::cout << glm::to_string(v2.position_homo) << std::endl;
            
            // do clipping
            // TODO: support other primitives
            std::vector<Vertex> out_vertices;
            clippTriangle(v0, v1, v2, out_vertices);

            int out_size = out_vertices.size();

            // out vertices <= 5
            assert(out_size <= 5 && "!!! clipped tiranle generate > 5 vertices");

            // culled
            if (out_size == 0)
                return;

            // do screen mapping
            // TODO: 不知clipping和screen mapping是串行还是并发比较好
            std::cout << "after clip:" << std::endl;
            for (int i = 0; i < out_size; i++)
            {   
                std::cout << glm::to_string(out_vertices[i].position_homo) << std::endl;
                mapScreen(out_vertices[i]);
            }
            
            std::copy(
                out_vertices.begin(), 
                out_vertices.end(),
                std::back_inserter(g_pipeline_global_context.geometry_vertex_buffer));

            std::cout << g_pipeline_global_context.geometry_vertex_buffer.size() << std::endl;

            for (int i = 1; i < out_size - 1; i++)
            {
                g_pipeline_global_context.geometry_index_buffer.push_back(out_index_base);
                g_pipeline_global_context.geometry_index_buffer.push_back(out_index_base + i);
                g_pipeline_global_context.geometry_index_buffer.push_back(out_index_base + i + 1);
            }

            out_index_base += out_size;
        }
    }

    void GeometryProcessor::mapScreen(Vertex& v)
    {
        v.position_homo = screen_mapping_matrix * v.position_homo;
    }

    /** clip in right-handed homogeneous space: [-1, 1]^3, with z flipped
     *  clip against near and far plane
     *  cull against xy screen
    */
    void GeometryProcessor::clippTriangle(Vertex& v0, Vertex& v1, Vertex& v2, std::vector<Vertex>& out_vertices)
    {   
        glm::vec4 p0 = v0.position_homo;
        glm::vec4 p1 = v1.position_homo;
        glm::vec4 p2 = v2.position_homo;

        // cull against xy screen
        if (p0.x < -p0.w && p1.x < -p1.w && p2.x < -p2.w)
            return;
        if (p0.x > p0.w && p1.x > p1.w && p2.x > p2.w)
            return;
        if (p0.y < -p0.w && p1.y < -p1.w && p2.y < -p2.w)
            return;
        if (p0.y > p0.w && p1.y > p1.w && p2.y > p2.w)
            return;
        
        // clip against near (z > -1 * w) and far plane (z < 1 * w) in homogeneous space
        uint8_t code0 = 0;
        uint8_t code1 = 0;
        uint8_t code2 = 0;

        // setup
        // 10 | 00 | 01
        if (p0.z < -p0.w)
            code0 |= 0x02;
        else if (p0.z > p0.w)
            code0 |= 0x01;
        if (p1.z < -p1.w)
            code1 |= 0x02;
        else if (p1.z > p1.w)
            code1 |= 0x01;
        if (p2.z < -p2.w)
            code2 |= 0x02;
        else if (p2.z > p2.w)
            code2 |= 0x01;

        // culled
        if (code0 & code1 & code2) 
            return;

        // all in
        if (!(code0 | code1 | code2)) 
        {
            out_vertices.push_back(v0);
            out_vertices.push_back(v1);
            out_vertices.push_back(v2);
            return;
        }

        // need clipping
        if (!code0) // inside frustum
            out_vertices.push_back(v0);

        if (code1 != code0) // p0p1 needs clipping
        {   
            int       code01 = code0 | code1;
            float     t;
            glm::vec4 p0p1 = p1 - p0;

            if (code01 & 0x02) // clip near
            {
                t = (-p0.w - p0.z) / (p0p1.z + p0p1.w);
                Vertex v;
                v.position_homo = p0 + t * p0p1;

                // debug
                std::cout << glm::to_string(v.position_homo) << std::endl;

                v.normal = (1 - t) * v0.normal + t * v1.normal;
                v.uv = (1 - t) * v0.uv + t * v1.uv;
                out_vertices.push_back(v);
            }
            if (code01 & 0x01) // clip far
            {
                t = (p0.w - p0.z) / (p0p1.z - p0p1.w);
                Vertex v;
                v.position_homo = p0 + t * p0p1;
                v.normal = (1 - t) * v0.normal + t * v1.normal;
                v.uv = (1 - t) * v0.uv + t * v1.uv;
                out_vertices.push_back(v);
            }
            if (code01 == 0x03)
                if (code0 == 0x01) // need to clip far first
                {   
                    int index = out_vertices.size() - 1;
                    Vertex v = out_vertices[index];
                    out_vertices[index] = out_vertices[index - 1];
                    out_vertices[index - 1] = v; 
                }
        }
            
        if (!code1) // inside frustum
            out_vertices.push_back(v1);

        if (code2 != code1)
        {   
            int       code12 = code1 | code2;
            float     t;
            glm::vec4 p1p2 = p2 - p1;

            if (code12 & 0x02) // clip near
            {
                t = (-p1.w - p1.z) / (p1p2.z + p1p2.w);
                Vertex v;
                v.position_homo = p1 + t * p1p2;
                v.normal = (1 - t) * v1.normal + t * v2.normal;
                v.uv = (1 - t) * v1.uv + t * v2.uv;
                out_vertices.push_back(v);
            }
            if (code12 & 0x01) // clip far
            {
                t = (p1.w - p1.z) / (p1p2.z - p1p2.w);
                Vertex v;
                v.position_homo = p1 + t * p1p2;
                v.normal = (1 - t) * v1.normal + t * v2.normal;
                v.uv = (1 - t) * v1.uv + t * v2.uv;
                out_vertices.push_back(v);
            }
            if (code12 == 0x03)
                if (code1 == 0x01) // need to clip far first
                {   
                    int index = out_vertices.size() - 1;
                    Vertex v = out_vertices[index];
                    out_vertices[index] = out_vertices[index - 1];
                    out_vertices[index - 1] = v; 
                }
        }
        
        if (!code2) // inside frustum
            out_vertices.push_back(v2);

        if (code0 != code2)
        {   
            int       code20 = code2 | code0;
            float     t;
            glm::vec4 p2p0 = p0 - p2;

            if (code20 & 0x02) // clip near
            {
                t = (-p2.w - p2.z) / (p2p0.z + p2p0.w);
                Vertex v;
                v.position_homo = p2 + t * p2p0;
                v.normal = (1 - t) * v2.normal + t * v0.normal;
                v.uv = (1 - t) * v2.uv + t * v0.uv;
                out_vertices.push_back(v);
            }
            if (code20 & 0x01) // clip far
            {
                t = (p2.w - p2.z) / (p2p0.z - p2p0.w);
                Vertex v;
                v.position_homo = p2 + t * p2p0;
                v.normal = (1 - t) * v2.normal + t * v0.normal;
                v.uv = (1 - t) * v2.uv + t * v0.uv;
                out_vertices.push_back(v);
            }
            if (code20 == 0x03)
                if (code2 == 0x01) // need to clip far first
                {   
                    int index = out_vertices.size() - 1;
                    Vertex v = out_vertices[index];
                    out_vertices[index] = out_vertices[index - 1];
                    out_vertices[index - 1] = v; 
                }
        }
    }
}