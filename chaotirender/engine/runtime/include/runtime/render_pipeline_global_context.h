#pragma once

#include <runtime/tgaimage.h>
#include <runtime/mesh_data.h>
#include <runtime/fragment.h>

#include <glm/mat4x4.hpp>

#include <memory>

namespace Chaotirender
{
    class RenderPipelineGlobalContext
    {
    public:
        RenderPipelineGlobalContext(int w, int h);
        
        RenderPipelineGlobalContext() = default;

        const int screen_width;
        const int screen_height;

        TGAImage color_buffer;

        std::vector<Vertex>  vertex_buffer;
        std::vector<index_t> index_buffer; // 似乎不需要？读的时候就按顺序放了

        std::vector<Vertex> geometry_vertex_buffer;
        std::vector<index_t> geometry_index_buffer;

        std::vector<std::unique_ptr<FragmentBuffer>> fragment_buffers;

        TGAColor draw_color;
    };

    extern RenderPipelineGlobalContext g_pipeline_global_context;
}