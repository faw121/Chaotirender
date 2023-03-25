#pragma once

#include <runtime/render/light.h>
#include <runtime/render/camera.h>
#include <runtime/render/render_mesh.h>
#include <runtime/shader/simple_vertex_shader.h>
#include <runtime/shader/phong_pixel_shader.h>

#include <vector>
#include <memory>

// TODO:
// 0. make canvas bigger
// 1. visualize light, add ui to control light -> data structure for light (mesh)
// 2. solution for model with no .mtl, should give tex path
// 3. check phong shader, too dark ...


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

        AmbientLight m_ambient_light;
        DirectionalLight m_directional_light;
        PointLight m_point_light;

        std::vector<RenderMesh> m_render_mesh_list;

        std::shared_ptr<Chaotirender::SimpleVertexShader> m_simple_vertex_shader {nullptr};
        std::shared_ptr<Chaotirender::PhongPixelShader> m_phong_pixel_shader {nullptr};
    };
}