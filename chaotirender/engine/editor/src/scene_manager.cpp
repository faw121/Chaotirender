#include <editor/scene_manager.h>

#include <runtime/global/engine_global_context.h>

#include <iostream>

namespace Chaotirender
{
    void SceneManager::addObjectInstance()
    {   
        if (m_selected_obj_res_ind == -1)
            return;
        if (m_selected_obj_res_ind >= g_engine_global_context.m_asset_manager->m_object_resource_list.size())
        {
            std::cout << "--selected object resource index out of range\n";
            return;
        }
        RenderObjectResource& res = g_engine_global_context.m_asset_manager->m_object_resource_list[m_selected_obj_res_ind];
        std::string instance_name = res.m_name + std::to_string(res.m_instance_count);
        m_object_instance_list.push_back(RenderObjectInstance(res, instance_name));
        res.m_instance_count++;
    }
}