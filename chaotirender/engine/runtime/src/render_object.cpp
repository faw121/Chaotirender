#include <runtime/render_object.h>

namespace Chaotirender
{
    void RenderObjectResource::addDataToPipeline()
    {
        for (auto& mesh: mesh_list)
        {
            // add vertex and index buffer
            mesh.vertex_buffer_id =  g_render_pipeline.addVertexBuffer(mesh.vertex_buffer);
            mesh.index_buffer_id =  g_render_pipeline.addIndexBuffer(mesh.index_buffer);

            // if has texture, add texture
            if (mesh.material.ambient_texture.texels != nullptr)
                mesh.ambient_tex_id = g_render_pipeline.addTexture(
                    mesh.material.ambient_texture.width, mesh.material.ambient_texture.height, mesh.material.ambient_texture.channels,
                    mesh.material.ambient_texture.texels);
            
            if (mesh.material.diffuse_texture.texels != nullptr)
                mesh.diffuse_tex_id = g_render_pipeline.addTexture(
                    mesh.material.diffuse_texture.width, mesh.material.diffuse_texture.height, mesh.material.diffuse_texture.channels,
                    mesh.material.diffuse_texture.texels);
            
            if (mesh.material.specular_texture.texels != nullptr)
                mesh.specular_tex_id = g_render_pipeline.addTexture(
                    mesh.material.specular_texture.width, mesh.material.specular_texture.height, mesh.material.specular_texture.channels,
                    mesh.material.specular_texture.texels);
            
            if (mesh.material.normal_texture.texels != nullptr)
                mesh.normal_tex_id = g_render_pipeline.addTexture(
                    mesh.material.normal_texture.width, mesh.material.normal_texture.height, mesh.material.normal_texture.channels,
                    mesh.material.normal_texture.texels);
        }
    }

    Mesh::Mesh(RawMesh raw_mesh)
    {
        vertex_buffer_id = raw_mesh.vertex_buffer_id;
        index_buffer_id = raw_mesh.index_buffer_id;

        material.diffuse_tex_id = raw_mesh.diffuse_tex_id;

        material.ka = raw_mesh.material.ka;
        material.kd = raw_mesh.material.kd;
        material.ks = raw_mesh.material.ks;
        material.shininess = raw_mesh.material.shininess;
    }

    // TODO: should handle shader-specify data else-where
    void Mesh::bindDataToPipeline(SampleType tex_sample_type)
    {
        // vertex and index data
        g_render_pipeline.bindVertexBuffer(vertex_buffer_id);
        g_render_pipeline.bindIndexBuffer(index_buffer_id);

        // textures
        // TODO: just bind diffuse here
        if (material.diffuse_tex_id != -1)
            g_render_pipeline.bindPixelShaderTexture(material.diffuse_tex_id, "diffuse_texture", tex_sample_type); 
    }

    void Mesh::draw()
    {
        g_render_pipeline.draw();
    }

    RenderObjectInstance::RenderObjectInstance(RenderObjectResource resource)
    {
        for (auto& raw_mesh: resource.mesh_list)
        {
            mesh_list.push_back(Mesh(raw_mesh));
        }
    }
}