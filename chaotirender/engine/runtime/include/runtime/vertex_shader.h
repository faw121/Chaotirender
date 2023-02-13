#pragma once

#include <runtime/mesh_data.h>
#include <runtime/texture.h>

namespace Chaotirender
{
    class VertexShader
    {
    public:
        virtual void shadeVertex(Vertex& v) = 0;

        std::map<std::string, TextureBase*> texture_list;
    };
}
