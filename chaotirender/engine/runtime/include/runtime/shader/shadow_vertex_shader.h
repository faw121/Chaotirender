#pragma once

#include <glm/mat4x4.hpp>

#include <runtime/shader/base/vertex_shader.h>

namespace Chaotirender
{
    class ShadowVertexShader: public VertexShader
    {
    public:
        ShadowVertexShader();

        virtual void shadeVertex(Vertex& v) override;

        glm::mat4 model_matrix;
        glm::mat4 view_matrix;
        glm::mat4 projection_matrix;
    };
}