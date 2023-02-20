#include <runtime/rasterizer.h>
#include <runtime/render_pipeline_global_context.h>

#include <memory>

#include <runtime/debug.h>

#include <tbb/tbb.h>

#define TOPEDGE(e)  (e.x == 0 && e.y < 0)
#define LEFTEDGE(e) (e.x > 0)

#define FLOAT_EQUAL(x, c) c - 0.0001f < x && c + 0.0001f > x 

namespace Chaotirender
{   
    extern tbb::spin_mutex mtx;

    Rasterize::Rasterize(int w, int h): m_w(w), m_h(h) {}

    void Rasterize::triangleBoundingBox(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2)
    {
        float xmax_, xmin_, ymax_, ymin_;
        xmax_ = xmin_ = p0.x;
        ymax_ = ymin_ = p0.y;
        if (p1.x > xmax_)
            xmax_ = p1.x;
        if (p1.x < xmin_)
            xmin_ = p1.x;
        if (p1.y > ymax_)
            ymax_ = p1.y;
        if (p1.y < ymin_)
            ymin_ = p1.y;

        if (p2.x > xmax_)
            xmax_ = p2.x;
        if (p2.x < xmin_)
            xmin_ = p2.x;
        if (p2.y > ymax_)
            ymax_ = p2.y;
        if (p2.y < ymin_)
            ymin_ = p2.y;   

        m_bounding_box.xmin = xmin_ < 0 ? 0 : static_cast<int>(xmin_);
        m_bounding_box.xmax = xmax_ > m_w ? m_w - 1: static_cast<int>(xmax_);
        m_bounding_box.ymin = ymin_ < 0 ? 0 : static_cast<int>(ymin_);
        m_bounding_box.ymax = ymax_ > m_h ? m_h - 1: static_cast<int>(ymax_);
    }

    float Rasterize::evaluateEdgeEquation(float x, float y, EdgeEquation& e) { return x * e.x + y * e.y + e.z; }

    void Rasterize::triangleSetup(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2)
    {
        //edge equation
        m_e2 = EdgeEquation(p0.y - p1.y, -(p0.x - p1.x), -p0.x * (p0.y - p1.y) + p0.y * (p0.x - p1.x));
        m_e0 = EdgeEquation(p1.y - p2.y, -(p1.x - p2.x), -p1.x * (p1.y - p2.y) + p1.y * (p1.x - p2.x));
        m_e1 = EdgeEquation(p2.y - p0.y, -(p2.x - p0.x), -p2.x * (p2.y - p0.y) + p2.y * (p2.x - p0.x));

        // test inside, plug in p2 to e2
        if (evaluateEdgeEquation(p2.x, p2.y, m_e2) < 0)
        {
            m_e2 = - m_e2;
            m_e1 = - m_e1;
            m_e0 = - m_e0;
        }
    }

    bool Rasterize::triangleSetupCull(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2)
    {
        //edge equation, use counter-clock wise
        m_e2 = EdgeEquation(p0.y - p1.y, -(p0.x - p1.x), -p0.x * (p0.y - p1.y) + p0.y * (p0.x - p1.x));
        m_e0 = EdgeEquation(p1.y - p2.y, -(p1.x - p2.x), -p1.x * (p1.y - p2.y) + p1.y * (p1.x - p2.x));
        m_e1 = EdgeEquation(p2.y - p0.y, -(p2.x - p0.x), -p2.x * (p2.y - p0.y) + p2.y * (p2.x - p0.x));

        // test inside, plug in p2 to e2
        if (evaluateEdgeEquation(p2.x, p2.y, m_e2) < 0)
        {
            return false;
        }
        return true;
    }

    bool Rasterize::insideTriangle(float x, float y)
    {
        m_s0 = evaluateEdgeEquation(x, y, m_e0);

        if (m_s0 < 0 || (m_s0 == 0 && !TOPEDGE(m_e0) && !LEFTEDGE(m_e0)))
            return false;

        m_s1 = evaluateEdgeEquation(x, y, m_e1);

        if (m_s1 < 0 || (m_s1 == 0 && !TOPEDGE(m_e1) && !LEFTEDGE(m_e1)))
            return false;
        
        m_s2 = evaluateEdgeEquation(x, y, m_e2);
        
        if (m_s2 < 0 || (m_s2 == 0 && !TOPEDGE(m_e2) && !LEFTEDGE(m_e2)))
            return false;

        return true;
    }

    void Rasterize::swapi(int& x, int& y)
    {
        int temp = x;
        x = y;
        y = temp;
    }

    void Rasterize::barycentric()
    {
        // interpolate attributes
        float alpha, beta, gamma, s;
        s = m_s0 + m_s1 + m_s2;

        alpha = m_s0 / s;
        beta  = m_s1 / s;
        gamma = m_s2 / s;

        m_alphaw = alpha / m_w0;
        m_betaw = beta / m_w1;
        m_gammaw = gamma / m_w2;

        m_w_inverse = m_alphaw + m_betaw + m_gammaw;
    }

    void Rasterize::operator()(Triangle& triangle, std::vector<Fragment>& fragment_list)
    {
        glm::vec4 p0 = triangle.m_v0.position_homo;
        glm::vec4 p1 = triangle.m_v1.position_homo;
        glm::vec4 p2 = triangle.m_v2.position_homo;

        m_w0 = p0.w;
        m_w1 = p1.w;
        m_w2 = p2.w;

        // perspective division
        p0 /= p0.w;
        p1 /= p1.w;
        p2 /= p2.w; 

        triangleSetup(p0, p1, p2);

        // bounding box of triangle
        triangleBoundingBox(p0, p1, p2);

        // triangle traversal
        for (int i = m_bounding_box.xmin; i <= m_bounding_box.xmax; i++)
        {
            for (int j = m_bounding_box.ymin; j <= m_bounding_box.ymax; j++)
            {
                if (insideTriangle(i + 0.5f, j + 0.5f))
                {   
                    // g_pipeline_global_context.color_buffer.set(i, j, g_pipeline_global_context.draw_color);
                    Fragment fragment;
                    fragment.screen_coord = glm::i32vec2(i, j);

                    // interpolate attributes
                    barycentric();

                    fragment.depth = 1 / m_w_inverse; // absolute value

                    fragment.normal = (m_alphaw * triangle.m_v0.normal + m_betaw * triangle.m_v1.normal + m_gammaw * triangle.m_v2.normal) / m_w_inverse;

                    fragment.uv = (m_alphaw * triangle.m_v0.uv + m_betaw * triangle.m_v1.uv + m_gammaw * triangle.m_v2.uv) / m_w_inverse;

                    fragment_list.push_back(fragment);
                }
            }
        }
    }

    void Rasterize::operator()(float x0_, float y0_, float x1_, float y1_, Color color)
    {
        rasterizeLine(x0_, y0_, x1_, y1_, color);
    }

    void Rasterize::rasterizeLine(float x0_, float y0_, float x1_, float y1_, Color color)
    {
        int x0, x1, y0, y1, dx, dy, dx_abs, dy_abs;
        bool swapxy = false;

        x0 = static_cast<int>(x0_);
        x1 = static_cast<int>(x1_);
        y0 = static_cast<int>(y0_);
        y1 = static_cast<int>(y1_);
        dx = x1 - x0;
        dy = y1 - y0;

        dx_abs = abs(dx);
        dy_abs = abs(dy);

        if (dy_abs > dx_abs) // swap x, y
        {
            swapi(x0, y0);
            swapi(x1, y1);
            swapi(dy, dx);
            swapi(dx_abs, dy_abs);
            swapxy = true;
        }
        
        if (dx < 0) // swap p0, p1
        {
            swapi(x0, x1);
            swapi(y0, y1);
            dx = -dx;
            dy = -dy;
        }

        int err, deltax, deltay, sy, x, y;

        x = x0;
        y = y0;
        deltax = -2 * dy_abs;
        deltay = 2 * dx_abs;
        if (dy >= 0) // err <= 0
        {   
            err = dx_abs;
            sy = 1; 
        }
        else // -err <= 0
        {
            err = -dx_abs;
            sy = -1; 
        }
        for (x; x <= x1; x++)
        {   
            if (swapxy)
                g_render_pipeline.frame_buffer.setColor(y, x, color);
            else
                g_render_pipeline.frame_buffer.setColor(x, y, color);
            err += deltax;
            if (err <= 0)
            {
                y += sy;
                err += deltay;
            }
        }
    }

    void Rasterize::rasterizeAndDraw(Triangle& triangle)
    {
        glm::vec4 p0 = triangle.m_v0.position_homo;
        glm::vec4 p1 = triangle.m_v1.position_homo;
        glm::vec4 p2 = triangle.m_v2.position_homo;

        m_w0 = p0.w;
        m_w1 = p1.w;
        m_w2 = p2.w;

        // perspective division
        p0 /= p0.w;
        p1 /= p1.w;
        p2 /= p2.w; 

        triangleSetup(p0, p1, p2);

        // bounding box of triangle
        triangleBoundingBox(p0, p1, p2);

        // triangle traversal
        for (int i = m_bounding_box.xmin; i <= m_bounding_box.xmax; i++)
        {
            for (int j = m_bounding_box.ymin; j <= m_bounding_box.ymax; j++)
            {
                if (insideTriangle(i + 0.5f, j + 0.5f))
                {   
                    // g_pipeline_global_context.color_buffer.set(i, j, g_pipeline_global_context.draw_color);
                    Fragment fragment;
                    fragment.screen_coord = glm::vec2(i, j);

                    // interpolate attributes
                    barycentric();

                    fragment.depth = 1 / m_w_inverse; // absolute value

                    fragment.normal = (m_alphaw * triangle.m_v0.normal + m_betaw * triangle.m_v1.normal + m_gammaw * triangle.m_v2.normal) / m_w_inverse;

                    fragment.uv = (m_alphaw * triangle.m_v0.uv + m_betaw * triangle.m_v1.uv + m_gammaw * triangle.m_v2.uv) / m_w_inverse;

                    g_render_pipeline.m_pixel_shader->shadePixel(fragment);

                    if (fragment.depth < g_render_pipeline.frame_buffer.getDepth(i, j))
                    {   
                        // g_render_pipeline.m_pixel_shader->shadePixel(fragment);
                        g_render_pipeline.frame_buffer.setDepth(i, j, fragment.depth);
                        g_render_pipeline.frame_buffer.setColor(i, j, Color(static_cast<uint8_t>(fragment.color.z + 0.5f), static_cast<uint8_t>(fragment.color.y + 0.5f), static_cast<uint8_t>(fragment.color.x + 0.5f)));
                    }
                }
            }
        }
    }

    void Rasterize::rasterizeAndDrawWithCull(Triangle& triangle)
    {
        glm::vec4 p0 = triangle.m_v0.position_homo;
        glm::vec4 p1 = triangle.m_v1.position_homo;
        glm::vec4 p2 = triangle.m_v2.position_homo;

        m_w0 = p0.w;
        m_w1 = p1.w;
        m_w2 = p2.w;

        // perspective division
        p0 /= p0.w;
        p1 /= p1.w;
        p2 /= p2.w; 

        if (!triangleSetupCull(p0, p1, p2))
            return;

        // bounding box of triangle
        triangleBoundingBox(p0, p1, p2);

        // triangle traversal
        for (int i = m_bounding_box.xmin; i <= m_bounding_box.xmax; i++)
        {
            for (int j = m_bounding_box.ymin; j <= m_bounding_box.ymax; j++)
            {
                if (insideTriangle(i + 0.5f, j + 0.5f))
                {   
                    // g_pipeline_global_context.color_buffer.set(i, j, g_pipeline_global_context.draw_color);
                    Fragment fragment;
                    fragment.screen_coord = glm::vec2(i, j);

                    // interpolate attributes
                    barycentric();

                    fragment.depth = 1 / m_w_inverse; // absolute value

                    fragment.normal = (m_alphaw * triangle.m_v0.normal + m_betaw * triangle.m_v1.normal + m_gammaw * triangle.m_v2.normal) / m_w_inverse;

                    fragment.uv = (m_alphaw * triangle.m_v0.uv + m_betaw * triangle.m_v1.uv + m_gammaw * triangle.m_v2.uv) / m_w_inverse;

                    // g_render_pipeline.m_pixel_shader->shadePixel(fragment);

                    if (fragment.depth < g_render_pipeline.frame_buffer.getDepth(i, j))
                    {   
                        g_render_pipeline.m_pixel_shader->shadePixel(fragment);
                        g_render_pipeline.frame_buffer.setDepth(i, j, fragment.depth);
                        g_render_pipeline.frame_buffer.setColor(i, j, Color(static_cast<uint8_t>(fragment.color.z + 0.5f), static_cast<uint8_t>(fragment.color.y + 0.5f), static_cast<uint8_t>(fragment.color.x + 0.5f)));
                    }
                }
            }
        }
    }

    void Rasterize::rasterizeWithCull(Triangle& triangle, std::vector<Fragment>& fragment_list)
    {
        glm::vec4 p0 = triangle.m_v0.position_homo;
        glm::vec4 p1 = triangle.m_v1.position_homo;
        glm::vec4 p2 = triangle.m_v2.position_homo;

        m_w0 = p0.w;
        m_w1 = p1.w;
        m_w2 = p2.w;

        // perspective division
        p0 /= p0.w;
        p1 /= p1.w;
        p2 /= p2.w; 

        if (!triangleSetupCull(p0, p1, p2))
            return;

        // bounding box of triangle
        triangleBoundingBox(p0, p1, p2);

        // triangle traversal
        for (int i = m_bounding_box.xmin; i <= m_bounding_box.xmax; i++)
        {
            for (int j = m_bounding_box.ymin; j <= m_bounding_box.ymax; j++)
            {
                if (insideTriangle(i + 0.5f, j + 0.5f))
                {   
                    // g_pipeline_global_context.color_buffer.set(i, j, g_pipeline_global_context.draw_color);
                    Fragment fragment;
                    fragment.screen_coord = glm::vec2(i, j);

                    // interpolate attributes
                    barycentric();

                    fragment.depth = 1 / m_w_inverse; // absolute value

                    fragment.normal = (m_alphaw * triangle.m_v0.normal + m_betaw * triangle.m_v1.normal + m_gammaw * triangle.m_v2.normal) / m_w_inverse;

                    fragment.uv = (m_alphaw * triangle.m_v0.uv + m_betaw * triangle.m_v1.uv + m_gammaw * triangle.m_v2.uv) / m_w_inverse;

                    fragment_list.push_back(fragment);
                }
            }
        }
    }

    void Rasterize::rasterizeAndDrawWithLock(Triangle& triangle)
    {
        glm::vec4 p0 = triangle.m_v0.position_homo;
        glm::vec4 p1 = triangle.m_v1.position_homo;
        glm::vec4 p2 = triangle.m_v2.position_homo;

        m_w0 = p0.w;
        m_w1 = p1.w;
        m_w2 = p2.w;

        // perspective division
        p0 /= p0.w;
        p1 /= p1.w;
        p2 /= p2.w; 

        triangleSetup(p0, p1, p2);

        // bounding box of triangle
        triangleBoundingBox(p0, p1, p2);

        // triangle traversal
        for (int i = m_bounding_box.xmin; i <= m_bounding_box.xmax; i++)
        {
            for (int j = m_bounding_box.ymin; j <= m_bounding_box.ymax; j++)
            {
                if (insideTriangle(i + 0.5f, j + 0.5f))
                {   
                    // g_pipeline_global_context.color_buffer.set(i, j, g_pipeline_global_context.draw_color);
                    Fragment fragment;
                    fragment.screen_coord = glm::vec2(i, j);

                    // interpolate attributes
                    barycentric();

                    fragment.depth = 1 / m_w_inverse; // absolute value

                    fragment.normal = (m_alphaw * triangle.m_v0.normal + m_betaw * triangle.m_v1.normal + m_gammaw * triangle.m_v2.normal) / m_w_inverse;

                    fragment.uv = (m_alphaw * triangle.m_v0.uv + m_betaw * triangle.m_v1.uv + m_gammaw * triangle.m_v2.uv) / m_w_inverse;

                    g_render_pipeline.m_pixel_shader->shadePixel(fragment);
                    
                    {
                        tbb::spin_mutex::scoped_lock lock(mtx);
                        if (fragment.depth < g_render_pipeline.frame_buffer.getDepth(i, j))
                        {
                            g_render_pipeline.frame_buffer.setDepth(i, j, fragment.depth);
                            g_render_pipeline.frame_buffer.setColor(i, j, Color(static_cast<uint8_t>(fragment.color.z + 0.5f), static_cast<uint8_t>(fragment.color.y + 0.5f), static_cast<uint8_t>(fragment.color.x + 0.5f)));
                        }
                    }
                }
            }
        }
    }



    void Rasterizer::rasterizeLine(float x0_, float y0_, float x1_, float y1_)
    {
        int x0, x1, y0, y1, dx, dy, dx_abs, dy_abs;
        bool swapxy = false;

        x0 = static_cast<int>(x0_);
        x1 = static_cast<int>(x1_);
        y0 = static_cast<int>(y0_);
        y1 = static_cast<int>(y1_);
        dx = x1 - x0;
        dy = y1 - y0;

        dx_abs = abs(dx);
        dy_abs = abs(dy);

        if (dy_abs > dx_abs) // swap x, y
        {
            swapi(x0, y0);
            swapi(x1, y1);
            swapi(dy, dx);
            swapi(dx_abs, dy_abs);
            swapxy = true;
        }
        
        if (dx < 0) // swap p0, p1
        {
            swapi(x0, x1);
            swapi(y0, y1);
            dx = -dx;
            dy = -dy;
        }

        int err, deltax, deltay, sy, x, y;

        x = x0;
        y = y0;
        deltax = -2 * dy_abs;
        deltay = 2 * dx_abs;
        if (dy >= 0) // err <= 0
        {   
            err = dx_abs;
            sy = 1; 
        }
        else // -err <= 0
        {
            err = -dx_abs;
            sy = -1; 
        }
        for (x; x <= x1; x++)
        {   
            if (swapxy)
                g_pipeline_global_context.color_buffer.set(y, x, g_pipeline_global_context.draw_color);
            else
                g_pipeline_global_context.color_buffer.set(x, y, g_pipeline_global_context.draw_color);
            err += deltax;
            if (err <= 0)
            {
                y += sy;
                err += deltay;
            }
        }
    }

    void Rasterizer::rasterizeTriangle()
    {   
        int num_faces = g_pipeline_global_context.geometry_index_buffer.size() / 3;

        for (int f = 0; f < num_faces; f++)
        {   
            int index_base = f * 3;
            
            Vertex v0 = g_pipeline_global_context.geometry_vertex_buffer[g_pipeline_global_context.geometry_index_buffer[index_base + 0]];
            Vertex v1 = g_pipeline_global_context.geometry_vertex_buffer[g_pipeline_global_context.geometry_index_buffer[index_base + 1]];
            Vertex v2 = g_pipeline_global_context.geometry_vertex_buffer[g_pipeline_global_context.geometry_index_buffer[index_base + 2]];

            glm::vec4 p0 = v0.position_homo;
            glm::vec4 p1 = v1.position_homo;
            glm::vec4 p2 = v2.position_homo;

            float w0 = p0.w;
            float w1 = p1.w;
            float w2 = p2.w;

            // perspective division
            p0 /= p0.w;
            p1 /= p1.w;
            p2 /= p2.w; 

            EdgeEquation e0, e1, e2;
            float s0, s1, s2;

            triangleSetup(p0, p1, p2, e0, e1, e2);

            // bounding box of triangle
            TriangleBoundingBox bounding_box;
            triangleBoundingBox(p0, p1, p2, bounding_box);

            // triangle traversal
            for (int i = bounding_box.xmin; i <= bounding_box.xmax; i++)
            {
                for (int j = bounding_box.ymin; j <= bounding_box.ymax; j++)
                {
                    if (insideTriangle(i + 0.5f, j + 0.5f, s0, s1, s2, e0, e1, e2))
                    {   
                        // g_pipeline_global_context.color_buffer.set(i, j, g_pipeline_global_context.draw_color);
                        Fragment fragment;
                        fragment.screen_coord = glm::vec2(i, j);

                        // interpolate attributes
                        float alpha, beta, gamma, s;
                        s = s0 + s1 + s2;

                        alpha = s0 / s;
                        beta  = s1 / s;
                        gamma = s2 / s;

                        float w_inverse;

                        float alpha_, beta_, gamma_;
                        alpha_ = alpha / w0;
                        beta_ = beta / w1;
                        gamma_ = gamma / w2;

                        w_inverse = alpha_ + beta_ + gamma_;

                        fragment.depth = 1 / w_inverse; // absolute value

                        fragment.normal = (alpha_ * v0.normal + beta_ * v1.normal + gamma_ * v2.normal) / w_inverse;

                        fragment.uv = (alpha_ * v0.uv + beta_ * v1.uv + gamma_ * v2.uv) / w_inverse;

                        g_pipeline_global_context.fragment_buffer.push_back(fragment);
                    }
                }
            }
        }
    }

    float Rasterizer::evaluateEdgeEquation(float x, float y, EdgeEquation e) { return x * e.x + y * e.y + e.z; }

    void Rasterizer::swapi(int& x, int& y)
    {
        int temp = x;
        x = y;
        y = temp;
    }

    void Rasterizer::triangleBoundingBox(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, TriangleBoundingBox& bb)
    {
        float xmax_, xmin_, ymax_, ymin_;
        xmax_ = xmin_ = p0.x;
        ymax_ = ymin_ = p0.y;
        if (p1.x > xmax_)
            xmax_ = p1.x;
        if (p1.x < xmin_)
            xmin_ = p1.x;
        if (p1.y > ymax_)
            ymax_ = p1.y;
        if (p1.y < ymin_)
            ymin_ = p1.y;

        if (p2.x > xmax_)
            xmax_ = p2.x;
        if (p2.x < xmin_)
            xmin_ = p2.x;
        if (p2.y > ymax_)
            ymax_ = p2.y;
        if (p2.y < ymin_)
            ymin_ = p2.y;   

        bb.xmin = xmin_ < 0 ? 0 : static_cast<int>(xmin_);
        bb.xmax = xmax_ > g_pipeline_global_context.screen_width ? g_pipeline_global_context.screen_width - 1: static_cast<int>(xmax_);
        bb.ymin = ymin_ < 0 ? 0 : static_cast<int>(ymin_);
        bb.ymax = ymax_ > g_pipeline_global_context.screen_height ? g_pipeline_global_context.screen_height - 1: static_cast<int>(ymax_);
    }

    void Rasterizer::triangleSetup(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, EdgeEquation& e0, EdgeEquation& e1, EdgeEquation& e2)
    {
        //edge equation
        e2 = EdgeEquation(p0.y - p1.y, -(p0.x - p1.x), -p0.x * (p0.y - p1.y) + p0.y * (p0.x - p1.x));
        e0 = EdgeEquation(p1.y - p2.y, -(p1.x - p2.x), -p1.x * (p1.y - p2.y) + p1.y * (p1.x - p2.x));
        e1 = EdgeEquation(p2.y - p0.y, -(p2.x - p0.x), -p2.x * (p2.y - p0.y) + p2.y * (p2.x - p0.x));

        // test inside, plug in p2 to e2
        if (evaluateEdgeEquation(p2.x, p2.y, e2) < 0)
        {
            e2 = - e2;
            e1 = - e1;
            e0 = - e0;
        }
        
    }

    void Rasterizer::triangleSetupTile(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, EdgeEquation& e0, EdgeEquation& e1, EdgeEquation& e2, TileStep& ts0, TileStep& ts1, TileStep& ts2)
    {
        //edge equation
        e2 = EdgeEquation(p0.y - p1.y, -(p0.x - p1.x), -p0.x * (p0.y - p1.y) + p0.y * (p0.x - p1.x));
        e0 = EdgeEquation(p1.y - p2.y, -(p1.x - p2.x), -p1.x * (p1.y - p2.y) + p1.y * (p1.x - p2.x));
        e1 = EdgeEquation(p2.y - p0.y, -(p2.x - p0.x), -p2.x * (p2.y - p0.y) + p2.y * (p2.x - p0.x));

        // test inside, plug in p2 to e2
        if (evaluateEdgeEquation(p2.x, p2.y, e2) < 0)
        {
            e2 = - e2;
            e1 = - e1;
            e0 = - e0;
        }
        
        ts0.x = e0.x > 0 ? 1 : 0;
        ts0.y = e0.y > 0 ? 1 : 0;
        ts1.x = e1.x > 0 ? 1 : 0;
        ts1.y = e1.y > 0 ? 1 : 0;
        ts2.x = e2.x > 0 ? 1 : 0;
        ts2.y = e2.y > 0 ? 1 : 0;
    }

    bool Rasterizer::insideTriangle(float x, float y, float& s0, float& s1, float& s2, EdgeEquation& e0, EdgeEquation& e1, EdgeEquation& e2)
    {   
        s0 = evaluateEdgeEquation(x, y, e0);

        if (s0 < 0 || (s0 == 0 && !(TOPEDGE(e0)) && !(LEFTEDGE(e0))))
            return false;

        s1 = evaluateEdgeEquation(x, y, e1);

        if (s1 < 0 || (s1 == 0 && !(TOPEDGE(e1)) && !(LEFTEDGE(e1))))
            return false;
        
        s2 = evaluateEdgeEquation(x, y, e2);
        
        if (s2 < 0 || (s2 == 0 && !(TOPEDGE(e2)) && !(LEFTEDGE(e2))))
            return false;

        return true;
    }
}