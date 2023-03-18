#include <runtime/pipeline/process_geometry.h>

#include <runtime/debug.h>

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
        
        if (m_out_vertices.empty())
            return;

        // assemble and map
        assembleAndMap(triangle_group);
    }
    
    void ProcessGeometry::operator() (Triangle& t, LineGroup& line_group)
    {
        // vertex shading
        g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v0);
        g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v1);
        g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v2);

        // clip lines
        Line p0p1(t.m_v0.position_homo, t.m_v1.position_homo);
        Line p1p2(t.m_v1.position_homo, t.m_v2.position_homo);
        Line p2p0(t.m_v2.position_homo, t.m_v0.position_homo);

        if (clipLine(p0p1))
        {   
            mapScreen(p0p1.m_p0);
            mapScreen(p0p1.m_p1);
            line_group.lines.push_back(p0p1);
        }
        if (clipLine(p1p2))
        {   
            mapScreen(p1p2.m_p0);
            mapScreen(p1p2.m_p1);
            line_group.lines.push_back(p1p2);
        }
        if (clipLine(p2p0))
        {   
            mapScreen(p2p0.m_p0);
            mapScreen(p2p0.m_p1);
            line_group.lines.push_back(p2p0);
        }
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
                m_out_vertices.push_back(v[i]);

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

    bool ProcessGeometry::clipLine(Line& line)
    {   
        unsigned int code0 = computeCodeXYZ(line.m_p0);
        unsigned int code1 = computeCodeXYZ(line.m_p1);

        while(1)
        {   
            if (!(code0 | code1)) // all in
                break;
            if (code0 & code1) // all out
                return false; // not inside

            // need to clip
            // pick one end (bigger one is always outside)
            unsigned int code_end = code0 > code1 ? code0 : code1;
            glm::vec4 clipped_end;
            glm::vec4 p0p1 = line.m_p1 - line.m_p0;

            if (code_end & Top) //  y = w
            {
                float t = (line.m_p0.w - line.m_p0.y) / (p0p1.y - p0p1.w);
                clipped_end = line.m_p0 + t * p0p1;

            }
            else if (code_end & Bottom) // y = -w
            {
                float t = (-line.m_p0.w - line.m_p0.y) / (p0p1.y + p0p1.w);
                clipped_end = line.m_p0 + t * p0p1;
            }
            else if (code_end & Left) // x = -w
            {
                float t = (-line.m_p0.w - line.m_p0.x) / (p0p1.x + p0p1.w);
                clipped_end = line.m_p0 + t * p0p1;
            }
            else if (code_end & Right) // x = w
            {
                float t = (line.m_p0.w - line.m_p0.x) / (p0p1.x - p0p1.w);
                clipped_end = line.m_p0 + t * p0p1;
            }
            else if (code_end & Near) // z = -w
            {
                float t = (-line.m_p0.w - line.m_p0.z) / (p0p1.z + p0p1.w);
                clipped_end = line.m_p0 + t * p0p1;
            }
            else if (code_end & Far) // z= w
            {
                float t = (line.m_p0.w - line.m_p0.z) / (p0p1.z - p0p1.w);
                clipped_end = line.m_p0 + t * p0p1;
            }

            if (code_end == code0)
            {
                line.m_p0 = clipped_end;
                code0 = computeCodeXYZ(line.m_p0);
            }
            else if (code_end == code1)
            {
                line.m_p1 = clipped_end;
                code1 = computeCodeXYZ(line.m_p1);
            }
        }
        return true;
    }

    unsigned int ProcessGeometry::computeCodeXYZ(glm::vec4 p)
    {   
        unsigned int code = 0;
        if (p.x < -p.w - 0.001f)
            code |= Left;
        else if (p.x > p.w + 0.001f)
            code |= Right;
        if (p.y < -p.w - 0.001f)
            code |= Bottom;
        else if (p.y > p.w + 0.001f)
            code |= Top;
        if (p.z < -p.w - 0.001f)
            code |= Near;
        else if (p.z > p.w + 0.001f)
            code |= Far;

        return code;
    }

    void ProcessGeometry::mapScreen(glm::vec4& p)
    {
        p = g_render_pipeline.m_screen_mapping_matrix * p;
    }

    void ProcessGeometry::assembleAndMap(TriangleGroup& triangle_group)
    {   
        mapScreen(m_out_vertices[0].position_homo);
        mapScreen(m_out_vertices[1].position_homo);

		for (int i = 1; i < m_out_vertices.size() - 1; i++)
		{   
			mapScreen(m_out_vertices[i + 1].position_homo);
			triangle_group.triangles.push_back(Triangle(m_out_vertices[0], m_out_vertices[i], m_out_vertices[i + 1]));
		}
    }
}