#pragma once

#include <runtime/render_pipeline.h>
#include <runtime/asset_manager.h>
#include <runtime/render_resource.h>
#include <runtime/simple_vertex_shader.h>
#include <runtime/simple_pixel_shader.h>
#include <runtime/texture_pixel_shader.h>

#include <runtime/debug.h>
#include <runtime/tick_tock.h>

#include <tbb/tbb.h>

namespace Chaotirender
{  
    tbb::spin_mutex mtx;

    void clipTriangle(Triangle& triangle, std::vector<Vertex>& out_vertices)
    {  
        Vertex* v = (Vertex*) (&triangle);

        glm::vec4 p[3] = {triangle.m_v0.position_homo, triangle.m_v1.position_homo, triangle.m_v2.position_homo};

        // cull against xy screen
        if (p[0].x < -p[0].w && p[1].x < -p[1].w && p[2].x < -p[2].w)
                return;
        if (p[0].x > p[0].w && p[1].x > p[1].w && p[2].x > p[2].w)
            return;
        if (p[0].y < -p[0].w && p[1].y < -p[1].w && p[2].y < -p[2].w)
            return;
        if (p[0].y > p[0].w && p[1].y > p[1].w && p[2].y > p[2].w)
            return;

        // clip against near (z > -1 * w) and far plane (z < 1 * w) in homogeneous space
        uint8_t code[3] = {0, 0, 0};

        // setup
        // 10 | 00 | 01
        for (int i = 0; i < 3; i++)
        {
            if (p[i].z < -p[i].w)
                code[i] |= 0x02;
            else if (p[i].z > p[i].w)
                code[i] |= 0x01;
        }

        // culled
        if (code[0] & code[1] & code[2]) 
            return;

        // all in
        if (!(code[0] | code[1] | code[2])) 
        {  
            out_vertices.push_back(v[0]);
            out_vertices.push_back(v[1]);
            out_vertices.push_back(v[2]);
            return;
        }

        // need clipping
        for (int i = 0; i < 3; i++)
        {
            if (!code[i])
                out_vertices.push_back(triangle.m_v0);

            int j = (i + 1) % 3;
            if (code[j] != code[i]) // pipj needs clipping
            {
                int codeij = code[i] | code[j];
                float t;
                glm::vec4 pipj = p[j] - p[i];

                if (codeij & 0x02) // clip near
                {
                    t = (-p[i].w - p[i].z) / (pipj.z + pipj.w);
                    Vertex v_new;
                    v_new.position_homo = p[i] + t * pipj;
                    v_new.normal = (1 - t) * v[i].normal + t * v[j].normal;
                    v_new.uv = (1 - t) * v[i].uv + t * v[j].uv;
                    out_vertices.push_back(v_new);
                }
                if (codeij & 0x01) // clip far
                {
                    t = (p[i].w - p[i].z) / (pipj.z - pipj.w);
                    Vertex v_new;
                    v_new.position_homo = p[i] + t * pipj;
                    v_new.normal = (1 - t) * v[i].normal + t * v[j].normal;
                    v_new.uv = (1 - t) * v[i].uv + t * v[j].uv;
                    out_vertices.push_back(v_new);
                }
                if (codeij == 0x03)
                    if (code[i] == 0x01) // need to clip far first
                    {   
                        int index = out_vertices.size() - 1;
                        Vertex temp = out_vertices[index];
                        out_vertices[index] = out_vertices[index - 1];
                        out_vertices[index - 1] = temp; 
                    }
            }
        }
    }

    void mapScreen(Vertex& v)
    {
        v.position_homo = g_render_pipeline.screen_mapping_matrix * v.position_homo;
    }

	void assembleAndMap(std::vector<Vertex>& vertices, TriangleGroup& triangle_group)
	{   
        mapScreen(vertices[0]);
        mapScreen(vertices[1]);

		for (int i = 1; i < vertices.size() - 1; i++)
		{   
			mapScreen(vertices[i + 1]);
			triangle_group.triangles.push_back(Triangle(vertices[0], vertices[i], vertices[i + 1]));
		}
	}

    void setUp(VertexBuffer& vertex_buffer, IndexBuffer& index_buffer, SimpleVertexShader& simple_vertex_shader, TexturePixelShader& texture_pixel_shader)
    {
        loadAsset("asset/spot/spot_triangulated_good.obj");

        getDrawData(vertex_buffer, index_buffer);

        // simple_vertex_shader.model_matrix = glm::translate(simple_vertex_shader.model_matrix, glm::vec3(0, 0, -3));
        glm::mat4 model_mat(1), view_mat, projection_mat;

        model_mat = glm::rotate(model_mat, 3 * glm::pi<float>() / 4, glm::vec3(0, 1, 0));
        // model_mat = glm::rotate(model_mat, glm::pi<float>() / 4, glm::vec3(1, 0, 0));
        view_mat = glm::lookAt(glm::vec3(0, 0, 2.8f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        projection_mat = glm::perspective(glm::half_pi<float>() / 2, 1.f, 0.1f, 100.f); // glm::perspective(glm::half_pi<float>(), 1.f, 0.1f, 10.f);
        // projection_mat = glm::ortho(-2.f, 2.f, -2.f, 2.f, 0.1f, 100.f);

        simple_vertex_shader.model_matrix = model_mat;
        simple_vertex_shader.view_matrix = view_mat;
        simple_vertex_shader.projection_matrix = projection_mat;

        Light light;
        light.position = glm::vec3(0, 0, 12);
        light.color = glm::vec3(255, 255, 255);

        texture_pixel_shader.light = light;
    }

    void simpleRender()
    {
        VertexBuffer& vertex_buffer = *(g_render_pipeline.m_vertex_buffer);

        int num_faces = g_render_pipeline.m_index_buffer->size() / 3;

        for (int f = 0; f < num_faces; f++)
        {
            int index_base = 3 * f;
            Triangle t(vertex_buffer[index_base + 0], vertex_buffer[index_base + 1], vertex_buffer[index_base + 2]);

            t.m_v0.position_homo = glm::vec4(t.m_v0.position, 1);
            t.m_v1.position_homo = glm::vec4(t.m_v1.position, 1);
            t.m_v2.position_homo = glm::vec4(t.m_v2.position, 1);

            // vertex shading
            g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v0);
            g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v1);
            g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v2);

            // cliping
            std::vector<Vertex> out_vertices;
            clipTriangle(t, out_vertices);

            // assemble and map
            TriangleGroup triangle_group;
            assembleAndMap(out_vertices, triangle_group);

            // rasterize and draw
            for (auto& triangle: triangle_group.triangles)
            {   
                Rasterize rasterize(g_render_pipeline.m_w, g_render_pipeline.m_h);
                rasterize.rasterizeAndDraw(triangle);
            }
        }
    }

    void simpleRenderCull()
    {   
        VertexBuffer& vertex_buffer = *(g_render_pipeline.m_vertex_buffer);

        int num_faces = g_render_pipeline.m_index_buffer->size() / 3;

        for (int f = 0; f < num_faces; f++)
        {
            int index_base = 3 * f;
            Triangle t(vertex_buffer[index_base + 0], vertex_buffer[index_base + 1], vertex_buffer[index_base + 2]);

            t.m_v0.position_homo = glm::vec4(t.m_v0.position, 1);
            t.m_v1.position_homo = glm::vec4(t.m_v1.position, 1);
            t.m_v2.position_homo = glm::vec4(t.m_v2.position, 1);

            // vertex shading
            g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v0);
            g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v1);
            g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v2);

            // cliping
            std::vector<Vertex> out_vertices;
            clipTriangle(t, out_vertices);

            // assemble and map
            TriangleGroup triangle_group;
            assembleAndMap(out_vertices, triangle_group);

            // rasterize and draw
            for (auto& triangle: triangle_group.triangles)
            {   
                Rasterize rasterize(g_render_pipeline.m_w, g_render_pipeline.m_h);
                rasterize.rasterizeAndDrawWithCull(triangle);
            }
        }
    }
    
    void putFragmentsOut()
    {
        VertexBuffer& vertex_buffer = *(g_render_pipeline.m_vertex_buffer);

        int num_faces = g_render_pipeline.m_index_buffer->size() / 3;

        for (int f = 0; f < num_faces; f++)
        {
            int index_base = 3 * f;
            Triangle t(vertex_buffer[index_base + 0], vertex_buffer[index_base + 1], vertex_buffer[index_base + 2]);

            t.m_v0.position_homo = glm::vec4(t.m_v0.position, 1);
            t.m_v1.position_homo = glm::vec4(t.m_v1.position, 1);
            t.m_v2.position_homo = glm::vec4(t.m_v2.position, 1);

            // vertex shading
            g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v0);
            g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v1);
            g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v2);

            // cliping
            std::vector<Vertex> out_vertices;
            clipTriangle(t, out_vertices);

            // assemble and map
            TriangleGroup triangle_group;
            assembleAndMap(out_vertices, triangle_group);

            std::vector<Fragment> fragments;

            // rasterize
            for (auto& triangle: triangle_group.triangles)
            {   
                Rasterize rasterize(g_render_pipeline.m_w, g_render_pipeline.m_h);
                rasterize(triangle, fragments);
            }
            
            for (auto& fragment: fragments)
            {   
                g_render_pipeline.m_pixel_shader->shadePixel(fragment);
                int i = fragment.screen_coord.x;
                int j = fragment.screen_coord.y;
                if (fragment.depth < g_render_pipeline.frame_buffer.getDepth(i, j))
                {
                    g_render_pipeline.frame_buffer.setDepth(i, j, fragment.depth);
                    g_render_pipeline.frame_buffer.setColor(i, j, Color(static_cast<uint8_t>(fragment.color.z + 0.5f), static_cast<uint8_t>(fragment.color.y + 0.5f), static_cast<uint8_t>(fragment.color.x + 0.5f)));
                }
            }
        }
    }

    void parallelForRender()
    {
        std::vector<Triangle> triangle_list;
        VertexBuffer& vertex_buffer = *(g_render_pipeline.m_vertex_buffer);

        int num_faces = g_render_pipeline.m_index_buffer->size() / 3;

        for (int f = 0; f < num_faces; f++)
        {
            int index_base = 3 * f;
            Triangle t(vertex_buffer[index_base + 0], vertex_buffer[index_base + 1], vertex_buffer[index_base + 2]);

            t.m_v0.position_homo = glm::vec4(t.m_v0.position, 1);
            t.m_v1.position_homo = glm::vec4(t.m_v1.position, 1);
            t.m_v2.position_homo = glm::vec4(t.m_v2.position, 1);

            triangle_list.push_back(t);
        }

        tbb::parallel_for_each(triangle_list.begin(), triangle_list.end(),
        [&] (Triangle& t) -> void
        {
            // vertex shading
            g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v0);
            g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v1);
            g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v2);

            // cliping
            std::vector<Vertex> out_vertices;
            clipTriangle(t, out_vertices);

            // assemble and map
            TriangleGroup triangle_group;
            assembleAndMap(out_vertices, triangle_group);

            // rasterize and draw
            for (auto& triangle: triangle_group.triangles)
            {   
                Rasterize rasterize(g_render_pipeline.m_w, g_render_pipeline.m_h);
                rasterize.rasterizeAndDrawWithLock(triangle);
            }
        });
    }

    void seperateParallelFor()
    {
        std::vector<Triangle> triangle_list;
        VertexBuffer& vertex_buffer = *(g_render_pipeline.m_vertex_buffer);

        int num_faces = g_render_pipeline.m_index_buffer->size() / 3;

        for (int f = 0; f < num_faces; f++)
        {
            int index_base = 3 * f;
            Triangle t(vertex_buffer[index_base + 0], vertex_buffer[index_base + 1], vertex_buffer[index_base + 2]);

            t.m_v0.position_homo = glm::vec4(t.m_v0.position, 1);
            t.m_v1.position_homo = glm::vec4(t.m_v1.position, 1);
            t.m_v2.position_homo = glm::vec4(t.m_v2.position, 1);

            triangle_list.push_back(t);
        }

        std::vector<Triangle> triangles;

        TICK(geo)
        tbb::parallel_for_each(triangle_list.begin(), triangle_list.end(),
        [&] (Triangle& t) -> void
        {
            // vertex shading
            g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v0);
            g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v1);
            g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v2);

            // cliping
            std::vector<Vertex> out_vertices;
            clipTriangle(t, out_vertices);

            // assemble and map
            TriangleGroup triangle_group;
            assembleAndMap(out_vertices, triangle_group);

            {   
                tbb::spin_mutex::scoped_lock lock(mtx);
                std::copy(
                triangle_group.triangles.begin(), 
                triangle_group.triangles.end(),
                std::back_inserter(triangles));
            }
        });
        TOCK(geo)

        std::vector<Fragment> fragments;
        TICK(raster)
        tbb::parallel_for_each(triangles.begin(), triangles.end(),
        [&] (Triangle& t) -> void
        {
            Rasterize rasterize(g_render_pipeline.m_w, g_render_pipeline.m_h);
            rasterize(t, fragments);
        });
        TOCK(raster)

        TICK(shade)
        tbb::parallel_for_each(fragments.begin(), fragments.end(),
        [&] (Fragment& fragment) -> void
        {
            g_render_pipeline.m_pixel_shader->shadePixel(fragment);
            int i = fragment.screen_coord.x;
            int j = fragment.screen_coord.y;
            {
                tbb::spin_mutex::scoped_lock lock(mtx);
                if (fragment.depth < g_render_pipeline.frame_buffer.getDepth(i, j))
                {
                    g_render_pipeline.frame_buffer.setDepth(i, j, fragment.depth);
                    g_render_pipeline.frame_buffer.setColor(i, j, Color(static_cast<uint8_t>(fragment.color.z + 0.5f), static_cast<uint8_t>(fragment.color.y + 0.5f), static_cast<uint8_t>(fragment.color.x + 0.5f)));
                }
            } 
        });
        TOCK(shade)
    }

    void compare()
    {
        VertexBuffer& vertex_buffer = *(g_render_pipeline.m_vertex_buffer);

        TICK(serial_g)
        int num_faces = g_render_pipeline.m_index_buffer->size() / 3;

        for (int f = 0; f < num_faces; f++)
        {
            int index_base = 3 * f;
            Triangle t(vertex_buffer[index_base + 0], vertex_buffer[index_base + 1], vertex_buffer[index_base + 2]);

            t.m_v0.position_homo = glm::vec4(t.m_v0.position, 1);
            t.m_v1.position_homo = glm::vec4(t.m_v1.position, 1);
            t.m_v2.position_homo = glm::vec4(t.m_v2.position, 1);

            // vertex shading
            g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v0);
            g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v1);
            g_render_pipeline.m_vertex_shader->shadeVertex(t.m_v2);

            // cliping
            std::vector<Vertex> out_vertices;
            clipTriangle(t, out_vertices);

            // assemble and map
            TriangleGroup triangle_group;
            assembleAndMap(out_vertices, triangle_group);
        }
        TOCK(serial_g)
    }

    void pipelineRender()
    {   
        auto index_it = g_render_pipeline.m_index_buffer->begin();
        VertexBuffer& vertex_buffer = *(g_render_pipeline.m_vertex_buffer);
        int w = g_render_pipeline.m_w;
        int h = g_render_pipeline.m_h;

        tbb::filter<void, std::unique_ptr<Triangle>> assemble_triangle(tbb::filter_mode::serial_in_order, 
        [&] (tbb::flow_control &fc) -> std::unique_ptr<Triangle>
        {
            if (index_it == g_render_pipeline.m_index_buffer->end())
            {
                fc.stop();
                return nullptr;
            }

            // std::cout << "assemble" << std::endl;
            Vertex v0(vertex_buffer[*index_it]);
            Vertex v1(vertex_buffer[*++index_it]);
            Vertex v2(vertex_buffer[*++index_it]);

            v0.position_homo = glm::vec4(v0.position, 1);
            v1.position_homo = glm::vec4(v1.position, 1);
            v2.position_homo = glm::vec4(v2.position, 1);

            auto t = std::make_unique<Triangle>(v0, v1, v2);

            index_it++;
            return t;
        });

        tbb::filter<std::unique_ptr<Triangle>, std::unique_ptr<TriangleGroup>> process_geometry(tbb::filter_mode::parallel,
        [&] (std::unique_ptr<Triangle> t) -> std::unique_ptr<TriangleGroup>
        {   
            // std::cout << "shading" << std::endl;
            // vertex shading
            g_render_pipeline.m_vertex_shader->shadeVertex(t->m_v0);
            g_render_pipeline.m_vertex_shader->shadeVertex(t->m_v1);
            g_render_pipeline.m_vertex_shader->shadeVertex(t->m_v2);

            // cliping
            std::vector<Vertex> out_vertices;
            clipTriangle(*t, out_vertices);

            if (out_vertices.size() == 0)
                return nullptr;

            // assemble and map
            auto triangle_group = std::make_unique<TriangleGroup>();
            assembleAndMap(out_vertices, *triangle_group);

            return triangle_group;
        });

        tbb::filter<std::unique_ptr<TriangleGroup>, std::unique_ptr<std::vector<Fragment>>> rasterize_t(tbb::filter_mode::parallel,
        [&] (std::unique_ptr<TriangleGroup> triangle_group) -> std::unique_ptr<std::vector<Fragment>>
        {   
            if (triangle_group == nullptr)
                return nullptr;
            
            auto fragments = std::make_unique<std::vector<Fragment>>();
            for (auto& t: triangle_group->triangles)
            {
                Rasterize raster(w, h);
                // raster(t, *fragments);
                raster.rasterizeWithCull(t, *fragments);
            }

            return fragments;
        });

        tbb::filter<std::unique_ptr<std::vector<Fragment>>, std::unique_ptr<std::vector<Fragment>>> shade_pixel(tbb::filter_mode::parallel,
        [&] (std::unique_ptr<std::vector<Fragment>> fragments) -> std::unique_ptr<std::vector<Fragment>>
        {
            if (fragments == nullptr)
                return nullptr;
            
            for (auto& fragment: *fragments)
            {
                g_render_pipeline.m_pixel_shader->shadePixel(fragment);
            }
            // tbb::parallel_for_each(fragments->begin(), fragments->end(), 
            // [&] (Fragment& fragment) -> void
            // {
            //     g_render_pipeline.m_pixel_shader->shadePixel(fragment);
            // });

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
                if (fragment.depth < g_render_pipeline.frame_buffer.getDepth(i, j))
                {
                    g_render_pipeline.frame_buffer.setDepth(i, j, fragment.depth);
                    g_render_pipeline.frame_buffer.setColor(i, j, Color(static_cast<uint8_t>(fragment.color.z + 0.5f), static_cast<uint8_t>(fragment.color.y + 0.5f), static_cast<uint8_t>(fragment.color.x + 0.5f)));
                }
            }
        });

        tbb::filter<std::unique_ptr<std::vector<Fragment>>, void> early_zp(tbb::filter_mode::parallel,
        [&] (std::unique_ptr<std::vector<Fragment>> fragments) -> void
        {   
            if (fragments == nullptr)
                return;

            for (auto& fragment: *fragments)
            {   
                int i = fragment.screen_coord.x;
                int j = fragment.screen_coord.y;
                if (fragment.depth < g_render_pipeline.frame_buffer.getDepth(i, j))
                {   
                    g_render_pipeline.m_pixel_shader->shadePixel(fragment);

                    {
                        tbb::spin_mutex::scoped_lock lock(mtx);
                        g_render_pipeline.frame_buffer.setDepth(i, j, fragment.depth);
                        g_render_pipeline.frame_buffer.setColor(i, j, Color(static_cast<uint8_t>(fragment.color.z + 0.5f), static_cast<uint8_t>(fragment.color.y + 0.5f), static_cast<uint8_t>(fragment.color.x + 0.5f)));
                    }
                }
            }
        });

        tbb::filter<std::unique_ptr<std::vector<Fragment>>, void> early_z(tbb::filter_mode::serial_out_of_order,
        [&] (std::unique_ptr<std::vector<Fragment>> fragments) -> void
        {   
            if (fragments == nullptr)
                return;

            for (auto& fragment: *fragments)
            {   
                int i = fragment.screen_coord.x;
                int j = fragment.screen_coord.y;
                if (fragment.depth < g_render_pipeline.frame_buffer.getDepth(i, j))
                {   
                    g_render_pipeline.m_pixel_shader->shadePixel(fragment);
                    g_render_pipeline.frame_buffer.setDepth(i, j, fragment.depth);
                    g_render_pipeline.frame_buffer.setColor(i, j, Color(static_cast<uint8_t>(fragment.color.z + 0.5f), static_cast<uint8_t>(fragment.color.y + 0.5f), static_cast<uint8_t>(fragment.color.x + 0.5f)));
                }
            }
        });

        // tbb::filter<void, void> render_pipeline = assemble_triangle & process_geometry & rasterize_t & shade_pixel & merge_pixel;
        tbb::filter<void, void> early = assemble_triangle & process_geometry & rasterize_t & early_z;

        // tbb::parallel_pipeline(8, render_pipeline);
        tbb::parallel_pipeline(8, early);
    }

   void test()
	{
		auto vertex_buffer = std::make_unique<VertexBuffer>();
		auto index_buffer = std::make_unique<IndexBuffer>();

		auto simple_vertex_shader = std::make_shared<SimpleVertexShader>();
		auto texture_pixel_shader = std::make_shared<TexturePixelShader>();

		setUp(*vertex_buffer, *index_buffer, *simple_vertex_shader, *texture_pixel_shader);

		buffer_id vid = g_render_pipeline.addVertexBuffer(std::move(vertex_buffer));
		g_render_pipeline.bindVertexBuffer(vid);

		buffer_id iid = g_render_pipeline.addIndexBuffer(std::move(index_buffer));
		g_render_pipeline.bindIndexBuffer(iid);

        g_render_pipeline.setVertexShader(simple_vertex_shader);
        g_render_pipeline.setPixelShader(texture_pixel_shader);

		int w, h, n;
		RenderResource render_resource;
        // auto texture = render_resource.loadTexture("asset/spot/spot_texture.png");
        //  texture->m_sample_type = SampleType::BILINEAR;
        // texture_pixel_shader->texture = texture;
		auto raw = render_resource.loadRawTexture("asset/spot/spot_texture.png", w, h, n);
		auto id = g_render_pipeline.addTexture(w, h, n, raw);
		g_render_pipeline.bindPixelShaderTexture(id, "texture", SampleType::BILINEAR);

        // auto dt = 0;
        // auto t1 = std::chrono::steady_clock::now();
        TICK(simpleRender)
        for (int i = 0; i < 100; i++)
        {
            // simpleRender(); // 27ms with early z: 16ms
            // parallelForRender(); // 34ms
            // putFragmentsOut(); // 32ms
            // seperateParallelFor(); // 35ms
            // compare();
            pipelineRender(); // 18ms earlyz: 14ms, cull: 13ms, cull+early:13
            // simpleRenderCull(); // 14ms, earlyz:9ms
        }

        TOCKN(simpleRender, 100)    

        TGAImage out(g_render_pipeline.m_w, g_render_pipeline.m_h, 4, (const uint8_t*) g_render_pipeline.frame_buffer.getColorBuffer());

        out.write_tga_file("spotn.tga");
	}

   
}