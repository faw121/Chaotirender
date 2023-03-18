#pragma once

#include <runtime/pipeline/render_pipeline.h>
#include <runtime/asset_manager.h>
#include <runtime/render_resource.h>
#include <runtime/shader/simple_vertex_shader.h>
#include <runtime/shader/simple_pixel_shader.h>
#include <runtime/shader/texture_pixel_shader.h>

#include <runtime/resource/asset_manager.h>

#include <runtime/util/tgaimage.h>
#include <runtime/debug.h>
#include <runtime/tick_tock.h>

#include <tbb/tbb.h>

namespace Chaotirender
{  
    void testAssetManager()
    {
        AssetManager asset_manager;

        std::string object_resource_path("asset/");

        asset_manager.fetchObjectResourcesDesc(object_resource_path);

        for (int i = 0; i < 6; i++)
            asset_manager.loadObjectResource(i);
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
        light.intensity = glm::vec3(255, 255, 255);

        texture_pixel_shader.light = light;
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

        // g_render_pipeline.render_config.rasterize_config.primitive = PrimitiveType::line;
        // g_render_pipeline.render_config.rasterize_config.line_color = Color(255, 255, 255);
        // g_render_pipeline.render_config.enable_parallel = false;

        // g_render_pipeline.render_config.rasterize_config.back_face_culling = false;
        g_render_pipeline.render_config.shading_config.early_z = true;
        TICK(Render)
        g_render_pipeline.draw();
        TOCK(Render)

        TGAImage out(g_render_pipeline.frame_buffer.getWidth(), g_render_pipeline.frame_buffer.getHeight(), 4, (const uint8_t*) g_render_pipeline.frame_buffer.getColorBuffer());

        out.write_tga_file("spotnn.tga");
    }

    void initScene()
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
    }

    void drawAndGetScene(int& scene_w, int& scene_h, const uint8_t*& data)
	{   
        // TICK(clear)
        g_render_pipeline.frame_buffer.clear();

        // g_render_pipeline.render_config.enable_parallel = false;
        // g_render_pipeline.render_config.rasterize_config.back_face_culling = false;
        // g_render_pipeline.render_config.rasterize_config.primitive = PrimitiveType::line;
        // g_render_pipeline.render_config.rasterize_config.line_color = Color(255, 255, 255);

        g_render_pipeline.draw();        

        scene_w = g_render_pipeline.frame_buffer.getWidth();
        scene_h = g_render_pipeline.frame_buffer.getHeight();
        data = (const uint8_t*) g_render_pipeline.frame_buffer.getColorBuffer();
	}
}