#include <runtime/pipeline/data_type/frame_buffer.h>

#include <limits>

namespace Chaotirender
{
    const static Color BLACK = Color(0, 0, 0);
    const static float INF = std::numeric_limits<float>::infinity();

    FrameBuffer::FrameBuffer(int w, int h): m_w(w), m_h(h), m_color_buffer(w, h, BLACK), m_depth_buffer(w, h, INF) {}

    void FrameBuffer::clear()
    {
        m_color_buffer.clear(BLACK);
        m_depth_buffer.clear(INF);
    }

    void FrameBuffer::clear(const Color& clear_color)
    {
        m_color_buffer.clear(clear_color);
        m_depth_buffer.clear(INF);
    }

    void FrameBuffer::resizeAndClear(int w, int h)
    {
        m_w = w;
        m_h = h;
        m_color_buffer.resizeAndClear(w, h, BLACK);
        m_depth_buffer.resizeAndClear(w, h, INF);
    }

    void FrameBuffer::resizeAndClear(int w, int h, const Color& clear_color)
    {
        m_w = w;
        m_h = h;
        m_color_buffer.resizeAndClear(w, h, clear_color);
        m_depth_buffer.resizeAndClear(w, h, INF);
    }

    float FrameBuffer::getDepth(int i, int j) const { return m_depth_buffer.get(i, j); }
    Color FrameBuffer::getColor(int i, int j) const { return m_color_buffer.get(i, j); }

    void FrameBuffer::setDepth(int i, int j, float value) { m_depth_buffer.set(i, j, value); }
    void FrameBuffer::setColor(int i, int j, const Color& color) { m_color_buffer.set(i, j, color); }

    const Color* FrameBuffer::getColorBuffer() const { return m_color_buffer.data(); }
    const float* FrameBuffer::getDepthBuffer() const { return m_depth_buffer.data(); }
}