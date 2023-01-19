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

        void set_depth(int i, int j, float depth);

        float get_depth(int i, int j) const;

        const int screen_width;
        const int screen_height;

        TGAImage color_buffer;

        std::vector<Vertex>  vertex_buffer;
        std::vector<index_t> index_buffer;

        std::vector<Vertex> geometry_vertex_buffer;
        std::vector<index_t> geometry_index_buffer;

        std::vector<Fragment> fragment_buffer;

        TGAColor draw_color;
    
    private:
        std::vector<float> depth_buffer;
    };

    extern RenderPipelineGlobalContext g_pipeline_global_context;
}