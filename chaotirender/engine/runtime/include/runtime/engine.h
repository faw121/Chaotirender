#pragma once

#include <runtime/render/render_system.h>
#include <runtime/resource/asset_manager.h>
#include <runtime/resource/scene_manager.h>

#include <memory>

namespace Chaotirender
{
    class ChaotirenderEngine
    {
    public:
        // temporary
        ChaotirenderEngine();

        void init();

        void run();
        
    public:
        std::shared_ptr<RenderSystem> m_render_system;
        std::shared_ptr<AssetManager> m_asset_manager;
        std::shared_ptr<SceneManager> m_scene_manager;
    };
}