#include <runtime/pixel_processor.h>
#include <runtime/tgaimage.h>
#include <runtime/render_pipeline_global_context.h>

#include <cassert>

namespace Chaotirender
{   
    void PixelProcessor::processPixel()
    {
        assert(pixel_shader != nullptr && "no pixel shader is set!");

        for (auto fragment: g_pipeline_global_context.fragment_buffer)
        {
            pixel_shader->shadePixel(fragment);

            int i = fragment.screen_coord.x;
            int j = fragment.screen_coord.y;

            if (fragment.depth < g_pipeline_global_context.getDepth(i, j))
            {
                g_pipeline_global_context.setDepth(i, j, fragment.depth);

                g_pipeline_global_context.color_buffer.set(i, j, 
                    TGAColor(static_cast<uint8_t>(fragment.color.x + 0.5f), static_cast<uint8_t>(fragment.color.y + 0.5f), static_cast<uint8_t>(fragment.color.z + 0.5f)));
            }
        }
    }
}