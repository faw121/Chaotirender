#include <runtime/asset_manager.h>
#include <iostream>

namespace Chaotirender
{   
    tinyobj::ObjReader reader;

    bool loadAsset(std::string asset_path)
    {
        tinyobj::ObjReaderConfig reader_config;

        reader_config.mtl_search_path = "./";

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

    bool bindPipelineBuffer()
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
                        tu = attributes.texcoords[3 * size_t(index.texcoord_index) + 0];
                        tv = attributes.texcoords[3 * size_t(index.texcoord_index) + 1];
                    }
                    g_pipeline_global_context.vertex_buffer.push_back(Vertex(vx, vy, vz, nx, ny, nz, tu, tv));
                    g_pipeline_global_context.index_buffer.push_back(vertex_index);
                    vertex_index++;
                    // vertex_buffer.push_back(Vertex(vx, vy, vz, nx, ny, nz, tu, tv));
                }
                index_offset += num_f;
            }
        }
        return true;
    }
}