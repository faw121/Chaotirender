#include <runtime/render_pipeline_global_context.h>

#include <limits>

namespace Chaotirender
{   
    RenderPipelineGlobalContext g_pipeline_global_context(800, 800);
    
    RenderPipelineGlobalContext::RenderPipelineGlobalContext(int w, int h):
        screen_width(w), screen_height(h), color_buffer(w, h, TGAImage::RGB), depth_buffer(w * h)
    {
        draw_color = TGAColor(255, 255, 255, 255);

        std::fill(depth_buffer.begin(), depth_buffer.end(), std::numeric_limits<float>::infinity());
    }

    float RenderPipelineGlobalContext::get_depth(int i, int j) const
    {
        return depth_buffer[j * screen_width + i];
    }

    void RenderPipelineGlobalContext::set_depth(int i, int j, float depth)
    {
        depth_buffer[j * screen_width + i] =  depth;
    }

} // namespace Chaotirender
