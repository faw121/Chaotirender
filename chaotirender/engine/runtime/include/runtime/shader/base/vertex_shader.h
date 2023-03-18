#pragma once

#include <runtime/pipeline/data_type/pipeline_data_type.h>
#include <runtime/pipeline/data_type/texture.h>

namespace Chaotirender
{
    class VertexShader
    {
    public:
        virtual ~VertexShader() {};

        virtual void shadeVertex(Vertex& v) = 0;

        std::map<std::string, TextureBase*> texture_list;
    };
}
