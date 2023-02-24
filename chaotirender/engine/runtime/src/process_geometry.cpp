#include <runtime/process_geometry.h>

#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include <cassert>
#include <vector>
#include <algorithm>

namespace Chaotirender
{       
    void ProcessGeometry::operator() (Triangle& t, TriangleGroup& triangle_group)
    {
        // vertex shading
        g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v0);
        g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v1);
        g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v2);

        // cliping
        clipTriangle(t);

        // assemble and map
        assembleAndMap(triangle_group);
    }

    void ProcessGeometry::clipTriangle(Triangle& triangle)
    {
        Vertex* v = (Vertex*) (&triangle);

        glm::vec4 p[3] = {triangle.m_v0.position_homo, triangle.m_v1.position_homo, triangle.m_v2.position_homo};

        // cull against xy screen
        if (p[0].x < -p[0].w && p[1].x < -p[1].w && p[2].x < -p[2].w)
                return;
        if (p[0].x > p[0].w && p[1].x > p[1].w && p[2].x > p[2].w)
            return;
        if (p[0].y < -p[0].w && p[1].y < -p[1].w && p[2].y < -p[2].w)
            return;
        if (p[0].y > p[0].w && p[1].y > p[1].w && p[2].y > p[2].w)
            return;

        // clip against near (z > -1 * w) and far plane (z < 1 * w) in homogeneous space
        uint8_t code[3] = {0, 0, 0};

        // setup
        // 10 | 00 | 01
        for (int i = 0; i < 3; i++)
        {
            if (p[i].z < -p[i].w)
                code[i] |= 0x02;
            else if (p[i].z > p[i].w)
                code[i] |= 0x01;
        }

        // culled
        if (code[0] & code[1] & code[2]) 
            return;

        // all in
        if (!(code[0] | code[1] | code[2])) 
        {  
            m_out_vertices.push_back(v[0]);
            m_out_vertices.push_back(v[1]);
            m_out_vertices.push_back(v[2]);
            return;
        }

        // need clipping
        for (int i = 0; i < 3; i++)
        {
            if (!code[i])
                m_out_vertices.push_back(triangle.m_v0);

            int j = (i + 1) % 3;
            if (code[j] != code[i]) // pipj needs clipping
            {
                int codeij = code[i] | code[j];
                float t;
                glm::vec4 pipj = p[j] - p[i];

                if (codeij & 0x02) // clip near
                {
                    t = (-p[i].w - p[i].z) / (pipj.z + pipj.w);
                    Vertex v_new;
                    v_new.position_homo = p[i] + t * pipj;
                    v_new.normal = (1 - t) * v[i].normal + t * v[j].normal;
                    v_new.uv = (1 - t) * v[i].uv + t * v[j].uv;
                    m_out_vertices.push_back(v_new);
                }
                if (codeij & 0x01) // clip far
                {
                    t = (p[i].w - p[i].z) / (pipj.z - pipj.w);
                    Vertex v_new;
                    v_new.position_homo = p[i] + t * pipj;
                    v_new.normal = (1 - t) * v[i].normal + t * v[j].normal;
                    v_new.uv = (1 - t) * v[i].uv + t * v[j].uv;
                    m_out_vertices.push_back(v_new);
                }
                if (codeij == 0x03)
                    if (code[i] == 0x01) // need to clip far first
                    {   
                        int index = m_out_vertices.size() - 1;
                        Vertex temp = m_out_vertices[index];
                        m_out_vertices[index] = m_out_vertices[index - 1];
                        m_out_vertices[index - 1] = temp; 
                    }
            }
        }
    }

    void ProcessGeometry::mapScreen(Vertex& v)
    {
        v.position_homo = g_render_pipeline.m_screen_mapping_matrix * v.position_homo;
    }

    void ProcessGeometry::assembleAndMap(TriangleGroup& triangle_group)
    {
        mapScreen(m_out_vertices[0]);
        mapScreen(m_out_vertices[1]);

		for (int i = 1; i < m_out_vertices.size() - 1; i++)
		{   
			mapScreen(m_out_vertices[i + 1]);
			triangle_group.triangles.push_back(Triangle(m_out_vertices[0], m_out_vertices[i], m_out_vertices[i + 1]));
		}
    }
}