#include <runtime/global/engine_global_context.h>

namespace Chaotirender
{
    void EngineGlobalContext::clear()
    {
        m_asset_manager = nullptr;
        m_render_system = nullptr;
        m_window_system = nullptr;
    }

    EngineGlobalContext g_engine_global_context;
}