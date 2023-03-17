#pragma once

#include <runtime/resource/resource_type.h>

#include <glm/vec3.hpp>

#include <vector>
#include <string>

namespace Chaotirender
{   
    class RenderObjectResource
    {
    public:
        std::string m_path;
        // source file desc
        MeshSourceDesc m_mesh_source_desc;
        MaterialSourceDesc m_material_source_desc;

        // sub-mesh data
        std::vector<SubMesh> m_sub_mesh;
    };

    struct Transform
    {
        glm::vec3 translation {0.f, 0.f, 0.f};
        glm::vec3 rotation {0.f, 0.f, 0.f}; // not sure, rotate around model axis or world axis?
        glm::vec3 scale {1.f, 1.f, 1.f};
    };

    class RenderObjectInstance
    {
    public:
        RenderObjectInstance(const RenderObjectResource& resource);

    public:
        std::string m_name;

        Transform m_transform;

        PrimitiveType m_primitive_type {PrimitiveType::triangle};

        bool m_use_tex {true};

        std::vector<SubMesh> m_sub_mesh;
    };
};