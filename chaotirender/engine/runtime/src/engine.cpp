#include <runtime/engine.h>
#include <runtime/global/engine_global_context.h>

namespace Chaotirender
{   
    ChaotirenderEngine::ChaotirenderEngine()
    {
        m_render_system = std::make_shared<RenderSystem>();
        m_render_system->init();

        m_asset_manager = std::make_shared<AssetManager>();

        m_scene_manager = std::make_shared<SceneManager>();

        g_engine_global_context.m_render_system = m_render_system.get();
        g_engine_global_context.m_asset_manager = m_asset_manager.get();
        g_engine_global_context.m_scene_manager = m_scene_manager.get();
    }

    void ChaotirenderEngine::init()
    {
        m_render_system = std::make_shared<RenderSystem>();
        m_render_system->init();

        m_asset_manager = std::make_shared<AssetManager>();

        m_scene_manager = std::make_shared<SceneManager>();

        g_engine_global_context.m_render_system = m_render_system.get();
        g_engine_global_context.m_asset_manager = m_asset_manager.get();
        g_engine_global_context.m_scene_manager = m_scene_manager.get();
    }

    void ChaotirenderEngine::run()
    {   
        m_asset_manager->fetchObjectResourcesDesc("asset/");

        m_scene_manager->m_selected_obj_res_ind = 5;
        m_scene_manager->addObjectInstance(5, m_scene_manager->getNextObjInstanceName(5));

        // m_render_system->swapRenderData();
        // m_render_system->render();
    }
}