#pragma once

#include <runtime/resource/render_object.h>
#include <runtime/render/light.h>

#include <vector>

namespace Chaotirender
{   
    class SceneManager
    {
    public:
        void addObjectInstance(int ind, std::string name);
        std::string getNextObjInstanceName(int ind);

    public:
        std::vector<RenderObjectInstance> m_object_instance_list;

        int m_selected_obj_res_ind {-1};
        int m_selected_obj_ins_ind {-1};
    };
}