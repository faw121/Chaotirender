#pragma once

#include <runtime/resource/resource_type.h>

#include <glm/mat4x4.hpp>

namespace Chaotirender
{
    class RenderMesh
    {
    public:
        int m_vertex_buffer_id {-1};
        int m_index_buffer_id {-1};

        glm::mat4 m_model_mat;

        PrimitiveType m_primitive_type {PrimitiveType::triangle};

        bool m_use_tex {true};

        std::string m_material_type;

        MaterialTexId m_material_tex_id;

        PhongMaterial m_phong_material;
    
    public:
        RenderMesh(): m_model_mat(1.f) {}
    };
}