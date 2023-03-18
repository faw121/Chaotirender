#pragma once

namespace Chaotirender
{   
    struct EditorGlobalContextInitInfo
    {
        class WindowSystem* window_system;
        class EditorInputManager* input_manager;
        // class RenderSystem* render_system;
    };

    class EditorGlobalContext
    {
    public:
        class EditorInputManager* m_input_manager {nullptr};
        class WindowSystem* m_window_system {nullptr};
        // class RenderSystem* m_render_system {nullptr};

     public:
        void initialize(const EditorGlobalContextInitInfo& init_info);
        void clear();
    };

    extern EditorGlobalContext g_editor_global_context;
}