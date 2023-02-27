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
        void operator() (Triangle& t, LineGroup& line_group);

    private:
        void clipTriangle(Triangle& triangle);
        bool clipLine(Line& line);
        unsigned int computeCodeXYZ(glm::vec4 p);
        void assembleAndMap(TriangleGroup& triangle_group);
        void mapScreen(glm::vec4& p);

    private:
        // setup code     
        //                     / 100000 (far)
        //              000010
        //      000100    /      001000        
        //              000001   
        //  010000    /  (near)
        static constexpr unsigned int Top {2};
        static constexpr unsigned int Bottom {1};
        static constexpr unsigned int Left {4};
        static constexpr unsigned int Right {8};
        static constexpr unsigned int Near {16};
        static constexpr unsigned int Far {32};

    private:
        std::vector<Vertex> m_out_vertices;
    };
}