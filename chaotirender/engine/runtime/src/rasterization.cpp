#include <runtime/rasterization.h>

#include <memory>

#define TOPEDGE(e)  e.x == 0 && e.y < 0
#define LEFTEDGE(e) e.x > 0

namespace Chaotirender
{   
    extern std::vector<Chaotirender::Vertex>  vertex_buffer;
    extern std::vector<Chaotirender::index_t> index_buffer; 

    void rasterizeLine(float x0_, float y0_, float x1_, float y1_)
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

    EdgeEquation e0;
    EdgeEquation e1;
    EdgeEquation e2;

    float s0;
    float s1;
    float s2;

    void rasterizeTriangle()
    {   
        int num_faces = vertex_buffer.size() / 3;

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

            triangleSetup(p0, p1, p2);

            // bounding box of triangle
            TriangleBoundingBox bounding_box;
            triangleBoundingBox(p0, p1, p2, bounding_box);

            // fragment buffer
            auto fragment_buffer_ptr = std::make_unique<FragmentBuffer>(bounding_box);

            // triangle traversal
            for (int i = bounding_box.xmin; i <= bounding_box.xmax; i++)
            {
                for (int j = bounding_box.ymin; j <= bounding_box.ymax; j++)
                {
                    if (insideTriangle(i + 0.5f, j + 0.5f))
                    {
                        // interpolate attributes
                        // TODO: switch by perspective / orthogonal
                        Fragment fragment;
                        float alpha, beta, gamma, s;
                        s = s0 + s1 + s2;

                        alpha = s0 / s;
                        beta  = s1 / s;
                        gamma = s2 / s;

                        float w_inverse;
                        w_inverse = alpha / w0 + beta / w1 + gamma / w2;

                        float alpha_, beta_, gamma_;
                        alpha_ = alpha / w0;
                        beta_ = beta / w1;
                        gamma_ = gamma / w2;

                        fragment.depth = - 1 / w_inverse; // absolute value

                        fragment.normal = (alpha_ * v0.normal + beta_ * v1.normal + gamma_ * v2.normal) / w_inverse;
                        fragment.uv = (alpha_ * v0.uv + beta_ * v1.uv + gamma_ * v2.uv) / w_inverse;

                        fragment_buffer_ptr->setFragment(i, j, fragment);
                    }
                }
            }
            g_pipeline_global_context.fragment_buffers.push_back(std::move(fragment_buffer_ptr));
        }
    }

    void swapi(int& x, int& y)
    {
        int temp = x;
        x = y;
        y = temp;
    }

    void triangleBoundingBox(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, TriangleBoundingBox& bb)
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
        bb.xmax = xmax_ > g_pipeline_global_context.screen_width ? g_pipeline_global_context.screen_width : static_cast<int>(xmax_);
        bb.ymin = ymin_ < 0 ? 0 : static_cast<int>(ymin_);
        bb.ymax = ymax_ > g_pipeline_global_context.screen_height ? g_pipeline_global_context.screen_height : static_cast<int>(ymax_);
    }

    void triangleSetup(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2)
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

    bool insideTriangle(float x, float y)
    {
        s0 = evaluateEdgeEquation(x, y, e0);

        if (s0 < 0 || (s0 = 0 && !TOPEDGE(e0) && !LEFTEDGE(e0)))
            return false;

        s1 = evaluateEdgeEquation(x, y, e1);

        if (s1 < 0 || (s1 = 0 && !TOPEDGE(e1) && !LEFTEDGE(e1)))
            return false;
        
        s2 = evaluateEdgeEquation(x, y, e2);
        
        if (s2 < 0 || (s2 = 0 && !TOPEDGE(e2) && !LEFTEDGE(e2)))
            return false;

        return true;
    }
}