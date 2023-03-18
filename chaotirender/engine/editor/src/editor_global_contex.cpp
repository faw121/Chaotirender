#include <editor/editor_global_contex.h>

namespace Chaotirender
{   
    EditorGlobalContext g_editor_global_context;

    void EditorGlobalContext::initialize(const EditorGlobalContextInitInfo& init_info)
    {
        m_window_system  = init_info.window_system;
        m_input_manager = init_info.input_manager;
    }
}