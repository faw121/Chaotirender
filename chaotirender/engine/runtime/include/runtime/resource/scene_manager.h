#pragma once

#include <runtime/resource/render_object.h>
#include <runtime/render/light.h>

#include <glm/vec3.hpp>

#include <vector>
#include <memory>

namespace Chaotirender
{   
    class SceneManager
    {
    public:
        void addObjectInstance(int ind, std::string name);
        std::string getNextObjInstanceName(int ind);
        void removeObjectInstance(int ind);

    public:
        // light
        glm::vec3 m_ambient_intensity {50.f, 50.f, 50.f};

        float m_direction_pitch {0.f};
        float m_direction_yaw {0.f};
        glm::vec3 m_directional_intensity {100.f, 100.f, 100.f};

        float m_point_radius {50.f};
        float m_point_pitch {0.f};
        float m_point_yaw {0.f};
        glm::vec3 m_point_intensity {100.f, 100.f, 100.f};

        // object
        std::vector<RenderObjectInstance> m_object_instance_list;

        int m_selected_obj_res_ind {-1};
        int m_selected_obj_ins_ind {-1};
    };
}