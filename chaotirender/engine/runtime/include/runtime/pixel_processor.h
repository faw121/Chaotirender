#pragma once

#include <runtime/render_pipeline_global_context.h>
#include <runtime/pixel_shader.h>

namespace Chaotirender
{
    class PixelProcessor
    {
    public:
        void processPixel();

        void mergePixel();

        PixelShader* pixel_shader;
        
    };
}