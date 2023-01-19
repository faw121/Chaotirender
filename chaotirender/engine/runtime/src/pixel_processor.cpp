#include <runtime/pixel_processor.h>
#include <runtime/tgaimage.h>

namespace Chaotirender
{
    void PixelProcessor::mergePixel()
    {
        for (auto fragment: g_pipeline_global_context.fragment_buffer)
        {   
            int i = fragment.screen_coord.x;
            int j = fragment.screen_coord.y;

            if (fragment.depth < g_pipeline_global_context.get_depth(i, j))
            {
                g_pipeline_global_context.set_depth(i, j, fragment.depth);

                g_pipeline_global_context.color_buffer.set(i, j, 
                    TGAColor(static_cast<uint8_t>(fragment.color.x + 0.5f), static_cast<uint8_t>(fragment.color.y + 0.5f), static_cast<uint8_t>(fragment.color.z + 0.5f)));
            }
        }
    }
}