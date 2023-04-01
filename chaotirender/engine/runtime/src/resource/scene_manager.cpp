#include <runtime/resource/scene_manager.h>

#include <runtime/global/engine_global_context.h>

#include <iostream>

namespace Chaotirender
{   
    std::string SceneManager::getNextObjInstanceName(int ind)
    {
        RenderObjectResource& res = g_engine_global_context.m_asset_manager->m_object_resource_list[ind];
        return res.m_name + std::to_string(res.m_instance_count);
    }

    void SceneManager::addObjectInstance(int ind, std::string name)
    {   
        if (ind == -1)
            return;
        if (ind >= g_engine_global_context.m_asset_manager->m_object_resource_list.size())
        {
            std::cout << "--selected object resource index out of range\n";
            return;
        }
        RenderObjectResource& res = g_engine_global_context.m_asset_manager->m_object_resource_list[ind];

        // if not loaded yet, load and add to pipeline first
        if (!res.m_loaded)
        {   
            if (!g_engine_global_context.m_asset_manager->loadObjectResource(ind))
                return;
                
            g_engine_global_context.m_asset_manager->addObjectResourceToPipeline(ind);
        }

        m_object_instance_list.push_back(RenderObjectInstance());
        
        m_object_instance_list.back().m_name = name;
        m_object_instance_list.back().m_res_ind = ind;

        g_engine_global_context.m_asset_manager->createObjectInstance(ind, m_object_instance_list.back());
        
        res.m_instance_count++;
    }

    void SceneManager::removeObjectInstance(int ind)
    {
        if (ind == -1)
            return;
        if (ind >= m_object_instance_list.size())
        {
            std::cout << "--object resource index out of range\n";
            return;
        }
        
        m_selected_obj_ins_ind = -1;
        int res_ind = m_object_instance_list[ind].m_res_ind;
        m_object_instance_list.erase(m_object_instance_list.begin() + ind);
        g_engine_global_context.m_asset_manager->m_object_resource_list[res_ind].m_instance_count--;
    }
}