#include <runtime/resource/asset_manager.h>
#include <runtime/util/json.h>
#include <runtime/pipeline/render_pipeline.h>

#include <stb_image.h>

#include <filesystem>
#include <iostream>

namespace Chaotirender
{
    // scan object resource folder, for each resource, get mesh and material description
    void AssetManager::fetchObjectResourcesDesc(std::string object_resource_path)
    {
        m_object_resource_dir = object_resource_path;

        std::filesystem::directory_iterator dir_iterator(object_resource_path);

        for(auto& file_item: dir_iterator)
        {
            if (file_item.is_directory())
            {
                std::cout << "object resource: " + file_item.path().string() << std::endl;

                RenderObjectResource obj_resource;
                obj_resource.m_path = file_item.path().string();
                obj_resource.m_name = file_item.path().filename().string();

                // find .obj and .material.json file
                bool find_obj = false;
                bool find_material = false;
                for (auto &file : std::filesystem::directory_iterator(file_item.path()))
                {
                    if (std::filesystem::is_regular_file(file))
                    {   
                        auto file_path = file.path();
                        auto extension = file_path.extension();
                        // std::cout << "extension: " + extension.string() << std::endl;
                        if (find_obj && find_material)
                            break;

                        if (extension == ".obj")
                        {
                            obj_resource.m_mesh_source_desc.m_mesh_file = file_path.filename().string();
                            find_obj = true;
                        }

                        if (extension == ".json")
                        {   
                            if (file_path.stem().extension() == ".material")
                            {   
                                find_material = true;
                                // parse json file to MaterialSourceDesc
                                nlohmann::json json_object;
                                parseJSONFile(file_path.string(), json_object);

                                MaterialSourceDesc material_desc;
                                from_json(json_object, material_desc);
                                obj_resource.m_material_source_desc = material_desc;
                            }
                        }
                    }
                }
                m_object_resource_list.push_back(obj_resource);
            }
        }

        // debug
        for (auto& res: m_object_resource_list)
        {   
            std::cout << res.m_path << " -----" << std::endl;
            std::cout << res.m_mesh_source_desc.m_mesh_file << std::endl;
            std::cout << res.m_material_source_desc.m_base_color_file << std::endl;
            std::cout << res.m_material_source_desc.m_metallic_file << std::endl;
        }
    }

    void AssetManager::createObjectInstance(int res_index, RenderObjectInstance& obj_instance)
    {
        for (auto& mesh: m_object_resource_list[res_index].m_sub_mesh)
        {
            InstanceSubMesh instance_mesh;

            instance_mesh.m_mesh_id = m_mesh_asset_list[mesh.m_mesh_asset_ind].m_mesh_id;
            instance_mesh.m_mesh_size = m_mesh_asset_list[mesh.m_mesh_asset_ind].m_mesh_size;

            if (mesh.m_tex_asset_ind > -1) // if has texture
                instance_mesh.m_tex_id = m_tex_asset_list[mesh.m_tex_asset_ind].m_material_tex_id;

            instance_mesh.m_sub_material = mesh.m_sub_material;

            obj_instance.m_sub_mesh.push_back(instance_mesh);
        }
    }

    bool AssetManager::loadObjectResource(int res_index)
    {   
        if (res_index < 0 || res_index >= m_object_resource_list.size())
        {
            std::cout << "-- object resource index out of range" << std::endl;
            return false;
        }
        
        auto& obj_res = m_object_resource_list[res_index];

        // has mesh
        if (obj_res.m_mesh_source_desc.m_mesh_file.empty())
        {
            std::cout << "-- object resource has no mesh" << std::endl;
            return false;
        }

        // load mesh, if use .mtl, this process will load texture as well
        bool use_json_has_tex = !obj_res.m_material_source_desc.m_base_color_file.empty();

        loadMesh(obj_res);

        // if use .material.json, load texture
        if (use_json_has_tex)
            loadMaterialTexture(obj_res);
        
        obj_res.m_loaded = true;
        
        return true;
    }

    void AssetManager::loadMesh(RenderObjectResource& obj_res)
    {   
        tinyobj::ObjReaderConfig reader_config;

        std::string mesh_path = obj_res.m_path + "/" + obj_res.m_mesh_source_desc.m_mesh_file;
        
        std::filesystem::path res_folder(mesh_path);
        res_folder = res_folder.parent_path();

        // .mtl search path: current folder
        reader_config.mtl_search_path = res_folder.string();

        if (!obj_reader.ParseFromFile(mesh_path, reader_config)) 
        {
            if (!obj_reader.Error().empty())  
            {   
                std::cout << "failed to load obj file" << std::endl;
                std::cout << "TinyObjReader: " << obj_reader.Error();
            }
            return;
        }

        auto& attributes = obj_reader.GetAttrib();
        auto& shapes     = obj_reader.GetShapes();
        auto& materials  = obj_reader.GetMaterials();

        // for each shape
        for (size_t s = 0; s < shapes.size(); s++)
        {   
            // current offset
            size_t face_index_offset = 0;

            int vertex_index = 0;

            // sub mesh
            SubMesh sub_mesh;
            MeshAsset mesh_asset;
            mesh_asset.m_mesh_data.m_vertex_buffer = std::make_shared<VertexBuffer>();
            mesh_asset.m_mesh_data.m_index_buffer = std::make_shared<IndexBuffer>();

            int material_index = -1;

            // for each face
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
            {   
                size_t num_f = size_t(shapes[s].mesh.num_face_vertices[f]);
                // for each vertex in the face
                for (size_t v = 0; v < num_f; v++)
                {
                    // index of position, normal, texture
                    tinyobj::index_t index = shapes[s].mesh.indices[face_index_offset + v];

                    tinyobj::real_t vx = attributes.vertices[3 * size_t(index.vertex_index) + 0];
                    tinyobj::real_t vy = attributes.vertices[3 * size_t(index.vertex_index) + 1];
                    tinyobj::real_t vz = attributes.vertices[3 * size_t(index.vertex_index) + 2];
                    

                    // write to vertex buffer
                    // vertex_buffer.push_back()
                    tinyobj::real_t nx = -1;
                    tinyobj::real_t ny = -1;
                    tinyobj::real_t nz = -1;
                    if (index.normal_index >= 0)
                    {
                        nx = attributes.normals[3 * size_t(index.normal_index) + 0];
                        ny = attributes.normals[3 * size_t(index.normal_index) + 1];
                        nz = attributes.normals[3 * size_t(index.normal_index) + 2];
                    }

                    tinyobj::real_t tu = -1;
                    tinyobj::real_t tv = -1;
                    if (index.texcoord_index >= 0)
                    {
                        tu = attributes.texcoords[2 * size_t(index.texcoord_index) + 0];
                        tv = attributes.texcoords[2 * size_t(index.texcoord_index) + 1];
                    }

                    // push to asset list

                    mesh_asset.m_mesh_data.m_vertex_buffer->push_back(Vertex(vx, vy, vz, nx, ny, nz, tu, tv));
                    mesh_asset.m_mesh_data.m_index_buffer->push_back(vertex_index);
                    vertex_index++;
                }
                face_index_offset += num_f;
                material_index = shapes[s].mesh.material_ids[f];
            }
            if (material_index > -1) // use .mtl instead of .json
            {
                auto material = materials[material_index];
                sub_mesh.m_sub_material.m_phong_material.ka = glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]);
                sub_mesh.m_sub_material.m_phong_material.kd = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
                sub_mesh.m_sub_material.m_phong_material.ks = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);
                sub_mesh.m_sub_material.m_phong_material.shininess = material.shininess;

                sub_mesh.m_sub_material.m_use_tex = false;
                sub_mesh.m_sub_material.m_has_tex = false;

                // has tex
                if (!material.diffuse_texname.empty())
                {
                    obj_res.m_material_source_desc.m_base_color_file = material.diffuse_texname;
                    
                    // load tex to tex asset
                    m_tex_asset_list.push_back(MaterialTexAsset());
                    auto base_color_tex = std::make_shared<RawTexture>();
                    base_color_tex->m_texels = loadRawTexture(
                        obj_res.m_path + "/" + material.diffuse_texname, base_color_tex->m_width, base_color_tex->m_height, base_color_tex->m_channels);
                    m_tex_asset_list.back().m_material_tex_data.m_base_color_texture = base_color_tex;

                    sub_mesh.m_sub_material.m_use_tex = true;
                    sub_mesh.m_sub_material.m_has_tex = true;
                    sub_mesh.m_tex_asset_ind = m_tex_asset_list.size() - 1;
                }
            }
            // mesh size
            mesh_asset.m_mesh_size.m_num_vertices = mesh_asset.m_mesh_data.m_vertex_buffer->size();
            mesh_asset.m_mesh_size.m_num_triangles = mesh_asset.m_mesh_data.m_index_buffer->size() / 3;

            // add to asset list
            m_mesh_asset_list.push_back(mesh_asset);
            sub_mesh.m_mesh_asset_ind = m_mesh_asset_list.size() - 1;

            // add to sub mesh
            obj_res.m_sub_mesh.push_back(sub_mesh);
        }
    }

    void AssetManager::loadMaterialTexture(RenderObjectResource& obj_res)
    {   
        m_tex_asset_list.push_back(MaterialTexAsset());
        auto& material_asset = m_tex_asset_list.back();
        std::string res_folder = obj_res.m_path + "/";
        if (!obj_res.m_material_source_desc.m_base_color_file.empty())
        {   
            // use tex
            obj_res.m_sub_mesh[0].m_sub_material.m_use_tex = true;
            obj_res.m_sub_mesh[0].m_sub_material.m_has_tex = true;

            auto tex = std::make_shared<RawTexture>();
            tex->m_texels = loadRawTexture(res_folder + obj_res.m_material_source_desc.m_base_color_file, tex->m_width, tex->m_height, tex->m_channels);

            material_asset.m_material_tex_data.m_base_color_texture = tex;
        }
        if (!obj_res.m_material_source_desc.m_metallic_file.empty())
        {
            auto tex = std::make_shared<RawTexture>();
            tex->m_texels = loadRawTexture(res_folder + obj_res.m_material_source_desc.m_metallic_file, tex->m_width, tex->m_height, tex->m_channels);

            material_asset.m_material_tex_data.m_metallic_texture = tex;
        }
        if (!obj_res.m_material_source_desc.m_roughness_file.empty())
        {
            auto tex = std::make_shared<RawTexture>();
            tex->m_texels = loadRawTexture(res_folder + obj_res.m_material_source_desc.m_roughness_file, tex->m_width, tex->m_height, tex->m_channels);

            material_asset.m_material_tex_data.m_roughness_texture = tex;
        }
        if (!obj_res.m_material_source_desc.m_normal_file.empty())
        {
            auto tex = std::make_shared<RawTexture>();
            tex->m_texels = loadRawTexture(res_folder + obj_res.m_material_source_desc.m_normal_file, tex->m_width, tex->m_height, tex->m_channels);

            material_asset.m_material_tex_data.m_normal_texture = tex;
        }
        if (!obj_res.m_material_source_desc.m_occlusion_file.empty())
        {
            auto tex = std::make_shared<RawTexture>();
            tex->m_texels = loadRawTexture(res_folder + obj_res.m_material_source_desc.m_occlusion_file, tex->m_width, tex->m_height, tex->m_channels);

            material_asset.m_material_tex_data.m_occlusion_texture = tex;
        }
        if (!obj_res.m_material_source_desc.m_emissive_file.empty())
        {
            auto tex = std::make_shared<RawTexture>();
            tex->m_texels = loadRawTexture(res_folder + obj_res.m_material_source_desc.m_emissive_file, tex->m_width, tex->m_height, tex->m_channels);

            material_asset.m_material_tex_data.m_emissive_texture = tex;
        }
        obj_res.m_sub_mesh[0].m_tex_asset_ind = m_tex_asset_list.size() - 1;
    }

    uint8_t* AssetManager::loadRawTexture(std::string tex_file, int& w, int& h, int& n)
    {
        auto texels = static_cast<uint8_t*>(stbi_load(tex_file.c_str(), &w, &h, &n, 4));

        if (texels == nullptr)
        {
            std::cout << tex_file + ": " << "failed to load" << std::endl;
            return nullptr;
        }

        n = 4;
        // log
        std::cout << tex_file + ": " << "w:" << w << " h:" << h << " n:" << n << std::endl;
        return texels;
    }

    void AssetManager::addObjectResourceToPipeline(int res_index)
    {
        auto& obj_res = m_object_resource_list[res_index];
        if (!obj_res.m_loaded)
        {
            std::cout << "object resource not loaded yet!" << std::endl;
            return;
        }

        for (auto& submesh: obj_res.m_sub_mesh)
        {   
            addMeshDataToPipeline(submesh.m_mesh_asset_ind);
            addMaterialDataToPipeline(submesh.m_tex_asset_ind);
        }
    }

    void AssetManager::addMeshDataToPipeline(int asset_ind)
    {
        if (asset_ind < 0 || asset_ind >= m_mesh_asset_list.size())
        {
            std::cout << "-- mesh asset index out of range" << std::endl;
            return;
        }

        buffer_id v_id = g_render_pipeline.addVertexBuffer(*(m_mesh_asset_list[asset_ind].m_mesh_data.m_vertex_buffer));
        buffer_id i_id = g_render_pipeline.addIndexBuffer(*(m_mesh_asset_list[asset_ind].m_mesh_data.m_index_buffer));

        m_mesh_asset_list[asset_ind].m_mesh_id.m_vertex_buffer_id = v_id;
        m_mesh_asset_list[asset_ind].m_mesh_id.m_index_buffer_id = i_id;
    }

    void AssetManager::addMaterialDataToPipeline(int asset_ind)
    {
        if (asset_ind < 0 || asset_ind >= m_tex_asset_list.size())
        {
            std::cout << "-- mesh asset index out of range" << std::endl;
            return;
        }
        MaterialTexId& tex_id =  m_tex_asset_list[asset_ind].m_material_tex_id;
        MaterialTexData& tex_data =  m_tex_asset_list[asset_ind].m_material_tex_data;
        if (tex_data.m_base_color_texture != nullptr)
        {
            buffer_id id = g_render_pipeline.addTexture(
                tex_data.m_base_color_texture->m_width, tex_data.m_base_color_texture->m_height, tex_data.m_base_color_texture->m_channels, 
                tex_data.m_base_color_texture->m_texels);
            tex_id.m_base_color_tex_id = id;
        }
        if (tex_data.m_metallic_texture != nullptr)
        {
            buffer_id id = g_render_pipeline.addTexture(
                tex_data.m_metallic_texture->m_width, tex_data.m_metallic_texture->m_height, tex_data.m_metallic_texture->m_channels, 
                tex_data.m_metallic_texture->m_texels);
            tex_id.m_metallic_tex_id = id;
        }
            
        if (tex_data.m_roughness_texture != nullptr)
        {
            buffer_id id = g_render_pipeline.addTexture(
                tex_data.m_roughness_texture->m_width, tex_data.m_roughness_texture->m_height, tex_data.m_roughness_texture->m_channels, 
                tex_data.m_roughness_texture->m_texels);
            tex_id.m_roughness_tex_id = id;
        }
            
        if (tex_data.m_normal_texture != nullptr)
        {
            buffer_id id = g_render_pipeline.addTexture(
                tex_data.m_normal_texture->m_width, tex_data.m_normal_texture->m_height, tex_data.m_normal_texture->m_channels, 
                tex_data.m_normal_texture->m_texels);
            tex_id.m_normal_tex_id = id;
        }
        if (tex_data.m_occlusion_texture != nullptr)
        {
            buffer_id id = g_render_pipeline.addTexture(
                tex_data.m_occlusion_texture->m_width, tex_data.m_occlusion_texture->m_height, tex_data.m_occlusion_texture->m_channels, 
                tex_data.m_occlusion_texture->m_texels);
            tex_id.m_occlusion_tex_id = id;
        }
        if (tex_data.m_emissive_texture != nullptr)
        {
            buffer_id id = g_render_pipeline.addTexture(
                tex_data.m_emissive_texture->m_width, tex_data.m_emissive_texture->m_height, tex_data.m_emissive_texture->m_channels, 
                tex_data.m_emissive_texture->m_texels);
            tex_id.m_emissive_tex_id = id;
        }
    }
}