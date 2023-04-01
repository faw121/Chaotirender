#pragma once

#include <runtime/resource/resource_type.h>

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <string>

namespace Chaotirender
{   
    class RenderObjectResource
    {
    public:
        std::string m_path;
        std::string m_name;
        // source file desc
        MeshSourceDesc m_mesh_source_desc;
        MaterialSourceDesc m_material_source_desc;

        // sub-mesh data
        std::vector<SubMesh> m_sub_mesh;

        bool m_loaded {false};

        unsigned int m_instance_count {0};
    };

    struct Transform
    {
        glm::vec3 translation {0.f, 0.f, 0.f};
        glm::vec3 rotation {0.f, 0.f, 0.f}; // not sure, rotate around model axis or world axis?
        glm::quat rotation_q {1.f, 0.f, 0.f, 0.f};
        glm::vec3 scale {1.f, 1.f, 1.f};
    };

    class RenderObjectInstance
    {
    public:
        std::string m_name;

        int m_res_ind {-1};

        Transform m_transform;

        bool m_draw {true};

        PrimitiveType m_primitive_type {PrimitiveType::triangle};

        // bool m_use_tex {true};

        std::vector<InstanceSubMesh> m_sub_mesh;
    };
};