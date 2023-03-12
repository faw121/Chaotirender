#pragma once

#include <editor/camera.h>
#include <runtime/render_object.h>
#include <runtime/simple_vertex_shader.h>
#include <runtime/texture_pixel_shader.h>

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
        Camera camera;

        std::shared_ptr<Chaotirender::SimpleVertexShader> simple_vertex_shader {nullptr};
        std::shared_ptr<Chaotirender::TexturePixelShader> texture_pixel_shader {nullptr};

        std::vector<RenderObjectInstance> object_list;
    
    public:
        void draw();

    private:
        void drawCameraPass();
    };
}