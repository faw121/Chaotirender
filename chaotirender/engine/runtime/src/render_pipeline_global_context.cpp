#include <runtime/render_pipeline_global_context.h>

#include <limits>

#include <runtime/debug.h>

namespace Chaotirender
{   
    RenderPipelineGlobalContext g_pipeline_global_context(800, 800);

    void RenderPipelineGlobalContext::runPipeline()
    {
        geometry_processor->processGeometry();

        rasterizer->rasterizeTriangle();

        pixel_processor->processPixel();
    }
    
    void RenderPipelineGlobalContext::setVertexShader(VertexShader* vertex_shader_ptr)
    {
        geometry_processor->vertex_shader =vertex_shader_ptr;
    }

    void RenderPipelineGlobalContext::setPixelShader(PixelShader* pixel_shader_ptr)
    {
        pixel_processor->pixel_shader = pixel_shader_ptr;
    }

    void RenderPipelineGlobalContext::bindTexture(std::shared_ptr<TextureBase> texture_)
    {
        assert(pixel_processor->pixel_shader != nullptr && "pixel shader not set!");

        pixel_processor->pixel_shader->texture = texture_;
    }

    RenderPipelineGlobalContext::RenderPipelineGlobalContext(int w, int h):
        screen_width(w), screen_height(h), color_buffer(w, h, TGAImage::RGB), depth_buffer(w * h)
    {
        // pipeline parts
        geometry_processor = std::make_unique<GeometryProcessor>();
        geometry_processor->initialize(screen_width, screen_height);

        rasterizer = std::make_unique<Rasterizer>();

        pixel_processor = std::make_unique<PixelProcessor>();

        // initialize
        draw_color = TGAColor(255, 255, 255, 255);

        std::fill(depth_buffer.begin(), depth_buffer.end(), std::numeric_limits<float>::infinity());
    }

    float RenderPipelineGlobalContext::getDepth(int i, int j) const
    {
        return depth_buffer[j * screen_width + i];
    }

    void RenderPipelineGlobalContext::setDepth(int i, int j, float depth)
    {
        depth_buffer[j * screen_width + i] =  depth;
    }

} // namespace Chaotirender
