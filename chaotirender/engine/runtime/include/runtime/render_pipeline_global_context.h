#pragma once

#include <runtime/tgaimage.h>
#include <runtime/mesh_data.h>
#include <runtime/fragment.h>
#include <runtime/pixel_processor.h>
#include <runtime/geometry_processor.h>
#include <runtime/rasterizer.h>
#include <runtime/texture.h>

#include <glm/mat4x4.hpp>

#include <memory>

namespace Chaotirender
{
    class RenderPipelineGlobalContext
    {
    public:
        RenderPipelineGlobalContext(int w, int h);
        
        RenderPipelineGlobalContext() = default;

        void runPipeline();

        void setVertexShader(VertexShader* vertex_shader_ptr);

        void setPixelShader(PixelShader* pixel_shader_ptr);

        void bindTexture(std::shared_ptr<TextureBase> texture_);

        void setDepth(int i, int j, float depth);

        float getDepth(int i, int j) const;

    public:
        std::unique_ptr<GeometryProcessor> geometry_processor;

        std::unique_ptr<Rasterizer> rasterizer;

        std::unique_ptr<PixelProcessor> pixel_processor;

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