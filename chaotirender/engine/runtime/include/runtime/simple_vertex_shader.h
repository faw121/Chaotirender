#pragma once

#include <glm/mat4x4.hpp>

#include <runtime/vertex_shader.h>

namespace Chaotirender
{
    class SimpleVertexShader: public VertexShader
    {
    public:
        SimpleVertexShader();

        void shadeVertex(Vertex& v) override;

        glm::mat4 model_matrix;
        glm::mat4 view_matrix;
        glm::mat4 projection_matrix;
    };
}