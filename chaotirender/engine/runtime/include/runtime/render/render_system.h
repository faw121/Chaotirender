#pragma once

#include <runtime/render/camera.h>
#include <runtime/render/render_scene.h>
#include <runtime/pipeline/data_type/pipeline_data_type.h>

#include <runtime/shader/base/vertex_shader.h>
#include <runtime/shader/base/pixel_shader.h>

#include <memory>
#include <map>
#include <string>
#include <array>

namespace Chaotirender
{
    class RenderSystem
    {
    public:
        void init();

        void swapRenderData();

        void render();

        void setViewPort(int w, int h);

        std::array<int, 2> getViewPortSize() const;

        // TODO: this is for temporary
        void drawOneFrame(int& scene_w, int& scene_h, const uint8_t*& data);

        void drawOneFrame();
        
        void getDrawData(int& scene_w, int& scene_h, const uint8_t*& data);

    private:
        void drawCameraPass();

    public:
        RenderConfig m_render_config;

        SampleType m_sample_type {SampleType::BILINEAR};
        
        Camera m_camera;
        RenderScene m_render_scene;

        std::map<std::string, std::shared_ptr<VertexShader>> m_vertex_shader_map;
        std::map<std::string, std::shared_ptr<PixelShader>> m_pixel_shader_map;
    
    private:
        int m_w {1280};
        int m_h {720};
    };
}