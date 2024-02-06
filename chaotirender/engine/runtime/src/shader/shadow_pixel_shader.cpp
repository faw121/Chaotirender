#include <runtime/shader/shadow_pixel_shader.h>

namespace Chaotirender
{   
    glm::vec4 ShadowPixelShader::packDepth(float depth)
    {
        // 使用rgba 4字节共32位来存储z值,1个字节精度为1/256
        glm::vec4 bitShift(1.0, 256.0, 256.0 * 256.0, 256.0 * 256.0 * 256.0);
        glm::vec4 bitMask(1.0/256.0, 1.0/256.0, 1.0/256.0, 0.0);
        // gl_FragCoord:片元的坐标,fract():返回数值的小数部分
        glm::vec4 rgbaDepth = glm::fract(depth * bitShift); //计算每个点的z值
        rgbaDepth -= glm::vec4(rgbaDepth.g, rgbaDepth.b, rgbaDepth.a, rgbaDepth.a) * bitMask; // Cut off the value which do not fit in 8 bits
        return rgbaDepth;
    }

    void ShadowPixelShader::shadePixel(Fragment& fragment)
    {
        fragment.color = packDepth(fragment.depth);
    }
}