#pragma once

#include <runtime/pipeline/data_type/buffer.h>
#include <runtime/pipeline/data_type/color.h>

#include <string>

namespace Chaotirender
{
    using DepthBuffer = Buffer2D<float>;
    using ColorBuffer = Buffer2D<Color>;

    class FrameBuffer
    {
    public:
        FrameBuffer(int w, int h);

        float getDepth(int i, int j) const;
        Color getColor(int i, int j) const;

        void setDepth(int i, int j, float value);
        void setColor(int i, int j, const Color& value);

        void clear();
        void clear(const Color& clear_color);

        void resizeAndClear(int w, int h);
        void resizeAndClear(int w, int h, const Color& clear_color);

        int getWidth() { return m_w; }
        int getHeight() { return m_h; }

        const Color*  getColorBuffer() const;
        const float* getDepthBuffer() const;

    public:
        int m_w;
        int m_h;
        ColorBuffer m_color_buffer;
        DepthBuffer m_depth_buffer;
    };
}