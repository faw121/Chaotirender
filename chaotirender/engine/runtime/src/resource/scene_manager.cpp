#include <runtime/resource/scene_manager.h>

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

        // if not loaded yet, load and add to pipeline first
        if (!res.m_loaded)
        {   
            if (!g_engine_global_context.m_asset_manager->loadObjectResource(m_selected_obj_res_ind))
                return;
                
            g_engine_global_context.m_asset_manager->addObjectResourceToPipeline(m_selected_obj_res_ind);
        }

        std::string instance_name = res.m_name + std::to_string(res.m_instance_count);

        m_object_instance_list.push_back(RenderObjectInstance());
        g_engine_global_context.m_asset_manager->createObjectInstance(m_selected_obj_res_ind, m_object_instance_list.back());
        
        res.m_instance_count++;
    }
}