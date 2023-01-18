#pragma once

#include <runtime/mesh_data.h>

namespace Chaotirender
{
    class VertexShader
    {
    public:
        virtual void shadeVertex(Vertex& v) = 0;
    };
}
