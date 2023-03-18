#pragma once

#include <runtime/resource/asset_manager.h>
// #include <runtime/


namespace Chaotirender
{
    class EngineGlobalContext
    {
    public:  
        // asset manager
        AssetManager* m_asset_manager {nullptr};

        // render system
        class RenderSystem* m_render_system {nullptr};

        // window system
        class WindowSystem* m_window_system {nullptr};

    public:
        void clear();
    };
    extern EngineGlobalContext g_engine_global_context;
}