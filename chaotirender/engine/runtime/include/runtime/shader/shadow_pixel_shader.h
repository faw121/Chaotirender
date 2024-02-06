#pragma once

#include <runtime/shader/base/pixel_shader.h>

#include <glm/glm.hpp>

namespace Chaotirender
{   
    class ShadowPixelShader: public PixelShader
    {
    public:
        virtual void shadePixel(Fragment& fragment) override;

    private:
        static glm::vec4 packDepth(float depth);
    };
}