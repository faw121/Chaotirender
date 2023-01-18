#include <runtime/render_pipeline_global_context.h>

namespace Chaotirender
{   
    RenderPipelineGlobalContext g_pipeline_global_context(800, 800);
    
    RenderPipelineGlobalContext::RenderPipelineGlobalContext(int w, int h):screen_width(w), screen_height(h), color_buffer(TGAImage(w, h, TGAImage::RGB)) 
    {
        draw_color = TGAColor(255, 255, 255, 255);
    }

} // namespace Chaotirender
