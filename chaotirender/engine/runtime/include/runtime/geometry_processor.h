#pragma once

#include <vector>
#include <glm/mat4x4.hpp>

#include <runtime/mesh_data.h>
#include <runtime/vertex_shader.h>

namespace Chaotirender
{   
    class GeometryProcessor
    {
    public:
        void processGeometry();

        void initialize(int w, int h);

        void clippTriangle(Vertex& v0, Vertex& v1, Vertex& v2, std::vector<Vertex>& out_vertices);

        void mapScreen(Vertex& v);

        VertexShader* vertex_shader;

    private:
        glm::mat4 screen_mapping_matrix;
    };
}