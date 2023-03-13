#include <runtime/asset_manager.h>

#include <stb_image.h>

#include <iostream>

#include <runtime/debug.h>

namespace Chaotirender
{   
    bool AssetManager::loadAsset(std::string asset_path)
    {
        tinyobj::ObjReaderConfig reader_config;

        m_asset_folder = asset_path;
        m_asset_folder = m_asset_folder.parent_path();
        // std::filesystem::path asset_folder(asset_path);
        // asset_folder = asset_folder.parent_path();

        // .mtl search path: current folder
        reader_config.mtl_search_path = m_asset_folder.string();

        if (!obj_reader.ParseFromFile(asset_path, reader_config)) 
        {
            if (!obj_reader.Error().empty())  
            {
                std::cerr << "TinyObjReader: " << obj_reader.Error();
            }
            return false;
        }
        return true;
    }

    void AssetManager::getObjectResource(std::string asset_path, RenderObjectResource& resource)
    {   
        resource.name = asset_path;
        if (loadAsset(asset_path))
        {
            getMeshData(resource);

            getTextureData(resource);
        }
    }

    void AssetManager::getTextureData(RenderObjectResource& object_resource)
    {
        for (auto& mesh: object_resource.mesh_list)
        {
            std::string tex_name = mesh.material.diffuse_texture.tex_file_name;
            std::string tex_path = m_asset_folder.string() + "/" + tex_name;
            if (!tex_name.empty())
                mesh.material.diffuse_texture.texels = loadRawTexture(tex_path, 
                    mesh.material.diffuse_texture.width, mesh.material.diffuse_texture.height, mesh.material.diffuse_texture.channels);
        }
    }

    void AssetManager::getMeshData(RenderObjectResource& object_resource)
    {
        auto& attributes = obj_reader.GetAttrib();
        auto& shapes     = obj_reader.GetShapes();
        auto& materials  = obj_reader.GetMaterials();

        // for each shape
        for (size_t s = 0; s < shapes.size(); s++)
        {   
            // current offset
            size_t face_index_offset = 0;

            int vertex_index = 0;

            object_resource.mesh_list.push_back(RawMesh());

            RawMesh& mesh = object_resource.mesh_list[s];

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

                    mesh.vertex_buffer.push_back(Vertex(vx, vy, vz, nx, ny, nz, tu, tv));
                    mesh.index_buffer.push_back(vertex_index);
                    vertex_index++;
                }
                face_index_offset += num_f;
                material_index = shapes[s].mesh.material_ids[f];
            }
            if (material_index > -1)
            {
                auto material = materials[material_index];
                mesh.material.ka = glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]);
                mesh.material.kd = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
                mesh.material.ks = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);

                mesh.material.shininess = material.shininess;
                mesh.material.ambient_texture.tex_file_name = material.ambient_texname;
                mesh.material.diffuse_texture.tex_file_name = material.diffuse_texname;
                mesh.material.specular_texture.tex_file_name = material.specular_texname;
                mesh.material.normal_texture.tex_file_name = material.normal_texname;
            }
        }
    }

    uint8_t* AssetManager::loadRawTexture(std::string file, int& w, int&h, int& n)
    {
        auto texels = static_cast<uint8_t*>(stbi_load(file.c_str(), &w, &h, &n, 4));
        n = 4;
        // debug
        std::cout << "w:" << w << " h:" << h << " n:" << n << std::endl;
        return texels;
    }

    tinyobj::ObjReader reader;

    bool loadAsset(std::string asset_path)
    {
        tinyobj::ObjReaderConfig reader_config;

        std::filesystem::path asset_folder(asset_path);
        asset_folder = asset_folder.parent_path();

        reader_config.mtl_search_path = asset_folder.string();

        if (!reader.ParseFromFile(asset_path, reader_config)) 
        {
            if (!reader.Error().empty())  
            {
                std::cerr << "TinyObjReader: " << reader.Error();
            }
            return false;
        }
        return true;
    }

    bool getDrawData(std::vector<Vertex>& vertex_list, std::vector<index_t>& index_list)
    {
        auto& attributes = reader.GetAttrib();
        auto& shapes     = reader.GetShapes();
        auto& materials  = reader.GetMaterials();

        int vertex_index = 0;

        // for each shape
        for (size_t s = 0; s < shapes.size(); s++)
        {   
            // current offset
            size_t index_offset = 0;
            // for each face
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
            {   
                size_t num_f = size_t(shapes[s].mesh.num_face_vertices[f]);
                // for each vertex in the face
                for (size_t v = 0; v < num_f; v++)
                {
                    // index of position, normal, texture
                    tinyobj::index_t index = shapes[s].mesh.indices[index_offset + v];

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

                    vertex_list.push_back(Vertex(vx, vy, vz, nx, ny, nz, tu, tv));
                    index_list.push_back(vertex_index);
                    vertex_index++;
                }
                index_offset += num_f;
            }
        }
        return true;
    } 

    bool getMeshData(std::vector<RawMesh>& meshes)
    {
        auto& attributes = reader.GetAttrib();
        auto& shapes     = reader.GetShapes();
        auto& materials  = reader.GetMaterials();

        int vertex_index = 0;

        // for each shape
        for (size_t s = 0; s < shapes.size(); s++)
        {   
            // current offset
            size_t index_offset = 0;

            RawMesh mesh;
            VertexBuffer& vertex_buffer = mesh.vertex_buffer;
            IndexBuffer& index_buffer = mesh.index_buffer;
            int material_index = -1;

            // for each face
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
            {   
                size_t num_f = size_t(shapes[s].mesh.num_face_vertices[f]);
                // for each vertex in the face
                for (size_t v = 0; v < num_f; v++)
                {
                    // index of position, normal, texture
                    tinyobj::index_t index = shapes[s].mesh.indices[index_offset + v];

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

                    vertex_buffer.push_back(Vertex(vx, vy, vz, nx, ny, nz, tu, tv));
                    index_buffer.push_back(vertex_index);
                    vertex_index++;
                }
                index_offset += num_f;
                material_index = shapes[s].mesh.material_ids[f];
            }
            if (material_index > -1)
            {
                auto material = materials[material_index];
                mesh.material.ka = glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]);
                mesh.material.kd = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
                mesh.material.ks = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);

                mesh.material.shininess = material.shininess;
                mesh.material.ambient_texture.tex_file_name = material.ambient_texname;
                mesh.material.diffuse_texture.tex_file_name = material.diffuse_texname;
                mesh.material.specular_texture.tex_file_name = material.specular_texname;
                mesh.material.normal_texture.tex_file_name = material.normal_texname;
            }
            meshes.push_back(std::move(mesh));
        }
        return true;
    } 
}