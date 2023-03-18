#pragma once

#include <runtime/render/camera.h>
#include <runtime/render/render_scene.h>

// #include <runtime/

#include <memory>
#include <map>
#include <string>

namespace Chaotirender
{
    class RenderSystem
    {
    public:
        Camera m_camera;
        RenderScene m_render_scene;

        std::map<std::string, >
    };
}