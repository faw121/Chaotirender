#pragma once

#include <vector>
#include <glm/mat4x4.hpp>

#include <runtime/mesh_data.h>
#include <runtime/vertex_shader.h>
#include <runtime/render_pipeline.h>

namespace Chaotirender
{   
    // do vertex shading, clipping, and screen mapping
    class ProcessGeometry
    {
    public:
        // ProcessGeometry(int w, int h);
        ProcessGeometry() {}
        void operator() (Triangle& t, TriangleGroup& triangle_group);

    private:
        void clipTriangle(Triangle& triangle);
        void assembleAndMap(TriangleGroup& triangle_group);
        void mapScreen(Vertex& v);

    private:
        std::vector<Vertex> m_out_vertices;
    };
}