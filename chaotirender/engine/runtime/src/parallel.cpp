#include <runtime/parallel.h>

#include <runtime/debug.h>

namespace Chaotirender
{   
    // 1. 不正确：锁？merge？ 为啥慢？
    // 2. 一遍过的串行作为基准
    // 3. 整个塞进parallel for
    // 4. 部分优化


    void serial_render(std::vector<TriangleGroup>& triangle_groups)
    {
        auto index_it = g_pipeline_global_context.index_buffer.begin();
        while(index_it != g_pipeline_global_context.index_buffer.end())
        {
            Vertex v0(g_pipeline_global_context.vertex_buffer[*index_it]);
            Vertex v1(g_pipeline_global_context.vertex_buffer[*++index_it]);
            Vertex v2(g_pipeline_global_context.vertex_buffer[*++index_it]);

            v0.position_homo = glm::vec4(v0.position, 1);
            v1.position_homo = glm::vec4(v1.position, 1);
            v2.position_homo = glm::vec4(v2.position, 1);

        }
    }

    void parallel_render()
    {   
        auto index_it = g_pipeline_global_context.index_buffer.begin();

        tbb::filter<void, std::unique_ptr<Triangle>> assemble_triangle(tbb::filter_mode::serial_in_order, 
        [&] (tbb::flow_control &fc) -> std::unique_ptr<Triangle>
        {
            if (index_it == g_pipeline_global_context.index_buffer.end())
            {
                fc.stop();
                return nullptr;
            }
            // std::cout << "assemble" << std::endl;
            Vertex v0(g_pipeline_global_context.vertex_buffer[*index_it]);
            Vertex v1(g_pipeline_global_context.vertex_buffer[*++index_it]);
            Vertex v2(g_pipeline_global_context.vertex_buffer[*++index_it]);

            v0.position_homo = glm::vec4(v0.position, 1);
            v1.position_homo = glm::vec4(v1.position, 1);
            v2.position_homo = glm::vec4(v2.position, 1);

            auto t = std::make_unique<Triangle>(v0, v1, v2);

            index_it++;
            return std::move(t);
        });

        tbb::filter<std::unique_ptr<Triangle>, std::unique_ptr<Triangle>> shade_vertex(tbb::filter_mode::parallel,
        [&] (std::unique_ptr<Triangle> triangle) -> std::unique_ptr<Triangle>
        {   
            // std::cout << "shading" << std::endl;
            g_pipeline_global_context.geometry_processor->vertex_shader->shadeVertex(triangle->m_v0);
            g_pipeline_global_context.geometry_processor->vertex_shader->shadeVertex(triangle->m_v1);
            g_pipeline_global_context.geometry_processor->vertex_shader->shadeVertex(triangle->m_v2);
            return triangle;
        });

        tbb::filter<std::unique_ptr<Triangle>, std::unique_ptr<TriangleGroup>> clip_triangle(tbb::filter_mode::parallel,
        [&] (std::unique_ptr<Triangle> triangle) -> std::unique_ptr<TriangleGroup>
        {
            std::vector<Vertex> out_vertices;
            g_pipeline_global_context.geometry_processor->clippTriangle(triangle->m_v0, triangle->m_v1, triangle->m_v2, out_vertices);
            
            int size = out_vertices.size();
            if (size == 0)
                return nullptr;

            auto triangle_group = std::make_unique<TriangleGroup>();

            g_pipeline_global_context.geometry_processor->mapScreen(out_vertices[0]);
            g_pipeline_global_context.geometry_processor->mapScreen(out_vertices[1]);

            for (int i = 1; i < out_vertices.size() - 1; i++)
            {   
                g_pipeline_global_context.geometry_processor->mapScreen(out_vertices[i + 1]);
                triangle_group->triangles.push_back(Triangle(out_vertices[0], out_vertices[i], out_vertices[i + 1]));
            }

            return std::move(triangle_group);
        });

        tbb::filter<std::unique_ptr<TriangleGroup>, std::unique_ptr<std::vector<Fragment>>> rasterize(tbb::filter_mode::parallel,
        [&] (std::unique_ptr<TriangleGroup> triangle_group) -> std::unique_ptr<std::vector<Fragment>>
        {   
            if (triangle_group == nullptr)
                return nullptr;

            auto fragments = std::make_unique<std::vector<Fragment>>();

            tbb::spin_mutex mutex;

            for (auto& triangle: triangle_group->triangles)
            {
                // set up triangle
                glm::vec4 p0 = triangle.m_v0.position_homo;
                glm::vec4 p1 = triangle.m_v1.position_homo;
                glm::vec4 p2 = triangle.m_v2.position_homo;

                float w0 = p0.w;
                float w1 = p1.w;
                float w2 = p2.w;

                // perspective division
                p0 /= p0.w;
                p1 /= p1.w;
                p2 /= p2.w;

                EdgeEquation e0, e1, e2;
                // TileStep ts0, ts1, ts2;
                float s0, s1, s2;

                // g_pipeline_global_context.rasterizer->triangleSetupTile(p0, p1, p2, e0, e1, e2, ts0, ts1, ts2);
                g_pipeline_global_context.rasterizer->triangleSetup(p0, p1, p2, e0, e1, e2);

                // bounding box of triangle
                TriangleBoundingBox bounding_box;
                g_pipeline_global_context.rasterizer->triangleBoundingBox(p0, p1, p2, bounding_box); 


                for (int i = bounding_box.xmin; i <= bounding_box.xmax; i++)
                {
                    for (int j = bounding_box.ymin; j <= bounding_box.ymax; j++)
                    {
                        if (g_pipeline_global_context.rasterizer->insideTriangle(i + 0.5f, j + 0.5f, s0, s1, s2, e0, e1, e2))
                        {
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

                            fragment.normal = (alpha_ * triangle.m_v0.normal + beta_ * triangle.m_v1.normal + gamma_ * triangle.m_v2.normal) / w_inverse;

                            fragment.uv = (alpha_ * triangle.m_v0.uv + beta_ * triangle.m_v1.uv + gamma_ * triangle.m_v2.uv) / w_inverse;

                            fragments->push_back(fragment);
                        }
                    }
                }

                // tbb::parallel_for(tbb::blocked_range2d<size_t>(bounding_box.xmin, bounding_box.xmax + 1, bounding_box.ymin, bounding_box.ymax + 1),
                // [&] (tbb::blocked_range2d<size_t> r) -> void
                // {
                //     for (size_t i = r.rows().begin(); i < r.rows().end(); i++)
                //     {
                //         for (size_t j = r.cols().begin(); j < r.cols().end(); j++)
                //         {
                //             if (g_pipeline_global_context.rasterizer->insideTriangle(i + 0.5f, j + 0.5f, s0, s1, s2, e0, e1, e2))
                //             {
                //                 Fragment fragment;

                //                 fragment.screen_coord = glm::vec2(i, j);

                //                 // interpolate attributes
                //                 float alpha, beta, gamma, s;
                //                 s = s0 + s1 + s2;

                //                 alpha = s0 / s;
                //                 beta  = s1 / s;
                //                 gamma = s2 / s;

                //                 float w_inverse;

                //                 float alpha_, beta_, gamma_;
                //                 alpha_ = alpha / w0;
                //                 beta_ = beta / w1;
                //                 gamma_ = gamma / w2;

                //                 w_inverse = alpha_ + beta_ + gamma_;

                //                 fragment.depth = 1 / w_inverse; // absolute value

                //                 fragment.normal = (alpha_ * triangle.m_v0.normal + beta_ * triangle.m_v1.normal + gamma_ * triangle.m_v2.normal) / w_inverse;

                //                 fragment.uv = (alpha_ * triangle.m_v0.uv + beta_ * triangle.m_v1.uv + gamma_ * triangle.m_v2.uv) / w_inverse;

                //                 {
                //                     tbb::spin_mutex::scoped_lock lock(mutex);
                //                     fragments->push_back(fragment);
                //                 }
                //             }
                //         }
                //     }
                // });
            }
            return std::move(fragments);
        });

        tbb::filter<std::unique_ptr<std::vector<Fragment>>, std::unique_ptr<std::vector<Fragment>>> shade_pixel(tbb::filter_mode::parallel,
        [&] (std::unique_ptr<std::vector<Fragment>> fragments) -> std::unique_ptr<std::vector<Fragment>>
        {
            if (fragments == nullptr)
                return nullptr;
            
            // for(auto& fragment: *fragments)
            // {
            //     g_pipeline_global_context.pixel_processor->pixel_shader->shadePixel(fragment);
            // }
            tbb::parallel_for_each(fragments->begin(), fragments->end(), 
            [&] (Fragment& fragment) -> void
            {
                g_pipeline_global_context.pixel_processor->pixel_shader->shadePixel(fragment);
            });

            return fragments;
        });
        
        tbb::filter<std::unique_ptr<std::vector<Fragment>>, void> merge_pixel(tbb::filter_mode::serial_out_of_order,
        [&] (std::unique_ptr<std::vector<Fragment>> fragments) -> void
        {   
            if (fragments == nullptr)
                return;

            for (auto& fragment: *fragments)
            {   
                int i = fragment.screen_coord.x;
                int j = fragment.screen_coord.y;
                if (fragment.depth < g_pipeline_global_context.getDepth(i, j))
                {
                    g_pipeline_global_context.setDepth(i, j, fragment.depth);

                    g_pipeline_global_context.color_buffer.set(i, j, 
                        TGAColor(static_cast<uint8_t>(fragment.color.x + 0.5f), static_cast<uint8_t>(fragment.color.y + 0.5f), static_cast<uint8_t>(fragment.color.z + 0.5f)));
                }
            }
        });

        // tbb::filter<std::unique_ptr<TriangleGroup>, void> write_data(tbb::filter_mode::serial_in_order,
        // [&] (std::unique_ptr<TriangleGroup> triangle_group) -> void
        // {
        //     triangle_groups.push_back(*triangle_group);
        // });

        tbb::filter<void, void> render_pipeline = assemble_triangle & shade_vertex & clip_triangle & rasterize & shade_pixel & merge_pixel;

        tbb::parallel_pipeline(10, render_pipeline);
    }
}