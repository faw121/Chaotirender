#pragma once

#include <runtime/camera.h>
#include <runtime/render_object.h>
#include <runtime/simple_vertex_shader.h>
#include <runtime/phong_pixel_shader.h>

#include <vector>
#include <memory>

namespace Chaotirender
{   
    // TODO:
    // 1. RenderScene-RenderResource-RenderObjectInstance
    //                           -GlobalResource(skybox...)
    //      which class responsible for load?
    // 2. lights
    class RenderScene
    {
    public:
        void init();

    public:
        Camera m_camera;

        Light m_light;

        std::shared_ptr<Chaotirender::SimpleVertexShader> m_simple_vertex_shader {nullptr};
        std::shared_ptr<Chaotirender::PhongPixelShader> m_phong_pixel_shader {nullptr};

        std::vector<RenderObjectInstance> m_object_list;
    
    public:
        void draw();

    private:
        void drawCameraPass();
    };
}