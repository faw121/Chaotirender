#pragma once

#include <runtime/pipeline_data_type.h>

#include <runtime/util/json.h>

#include <vector>
#include <memory>
#include <string>

namespace Chaotirender
{   
    // static mesh data
    struct MeshData
    {
        std::shared_ptr<VertexBuffer> m_vertex_buffer {nullptr};
        std::shared_ptr<IndexBuffer>  m_index_buffer {nullptr};
    };

    struct RawTexture
    {
        int m_width;
        int m_height;
        int m_channels;
        uint8_t*  m_texels;  // TODO: may cause problem, since pipeline shares this data

        ~RawTexture()
        {
            if (m_texels)
                free(m_texels);
        }
    };

    // metallic-roughness structure
    struct MaterialTexData
    {   
        std::shared_ptr<RawTexture> m_base_color_texture;
        std::shared_ptr<RawTexture> m_metallic_texture;
        std::shared_ptr<RawTexture> m_roughness_texture;
        std::shared_ptr<RawTexture> m_normal_texture;
        std::shared_ptr<RawTexture> m_emissive_texture;
        std::shared_ptr<RawTexture> m_occlusion_texture;
    };

    struct MeshId
    {
        buffer_id m_vertex_buffer_id {-1};
        buffer_id m_index_buffer_id {-1};
    };

    // pbr material structure, phong just use base_color
    struct MaterialTexId
    {
        buffer_id m_base_color_tex_id {-1};
        buffer_id m_metallic_tex_id {-1};
        buffer_id m_roughness_tex_id {-1};
        buffer_id m_normal_tex_id {-1};
        buffer_id m_emissive_tex_id {-1};
        buffer_id m_occlusion_tex_id {-1};
    };

    // MeshData, MeshId, and MeshSize(int)
    struct MeshAsset
    {
        MeshData m_mesh_data;
        MeshId   m_mesh_id;

        // desc
        int num_vertices {0};
        int num_triangles {0};
    };

    // MaterialTexData, MaterialTexId
    struct MaterialTexAsset
    {
        MaterialTexData m_material_tex_data;
        MaterialTexId   m_material_tex_id;
    };

    struct PhongMaterial
    {   
        glm::vec3 ka {0.4f, 0.4f, 0.4f};
        glm::vec3 kd {0.7f, 0.7f, 0.7f};
        glm::vec3 ks {0.3f, 0.3f, 0.3f};
        float shininess {30.f};
    };

    // shading logic:
    // use_tex: false -> ignore type, just use phong shading
    //          true -> depends on material_type
    struct SubMeshMaterial
    {
        std::string m_material_type {"phong"}; // TODO: not sure, string or enum?
        bool m_use_tex; // if not use, use phong material for shading
        PhongMaterial m_phong_material;

        int m_tex_asset_ind {-1};
    };

    struct SubMesh
    {
        int m_mesh_asset_ind {-1};
        SubMeshMaterial m_sub_material;
    };

    struct MeshSourceDesc
    {
        std::string m_mesh_file;
    };

    struct MaterialSourceDesc
    {
        std::string m_material_type; // TODO: not sure, string or enum?
        
        std::string m_base_color_file;
        std::string m_metallic_file;
        std::string m_roughness_file;
        std::string m_normal_file;
        std::string m_emissive_file;
        std::string m_occlusion_file;

        friend void to_json(nlohmann::json& j, const MaterialSourceDesc& material_desc);
        friend void from_json(const nlohmann::json& j, MaterialSourceDesc& material_desc);
    };

    inline void to_json(nlohmann::json& j, const MaterialSourceDesc& material_desc)
    {
        j["base_color_file"] = material_desc.m_base_color_file;
        j["metallic_file"] = material_desc.m_metallic_file;
        j["roughness_file"] = material_desc.m_roughness_file;
        j["normal_file"] = material_desc.m_normal_file;
        j["occlusion_file"] = material_desc.m_occlusion_file;
        j["emissive_file"] = material_desc.m_emissive_file;
    }

    inline void from_json(const nlohmann::json& j, MaterialSourceDesc& material_desc)
    {
        material_desc.m_material_type = j.at("material_type").get<std::string>();
        material_desc.m_base_color_file = j.at("base_color_file").get<std::string>();
        material_desc.m_metallic_file = j.at("metallic_file").get<std::string>();
        material_desc.m_roughness_file = j.at("roughness_file").get<std::string>();
        material_desc.m_normal_file = j.at("normal_file").get<std::string>();
        material_desc.m_occlusion_file = j.at("occlusion_file").get<std::string>();
        material_desc.m_emissive_file = j.at("emissive_file").get<std::string>();
    }
}