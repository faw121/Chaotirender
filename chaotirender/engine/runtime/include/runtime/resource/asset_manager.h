#pragma once

#include <runtime/resource/resource_type.h>
#include <runtime/resource/render_object.h>

#include <tiny_obj_loader.h>

#include <vector>
#include <string>

namespace Chaotirender
{
    class AssetManager // TODO: delete another AssetManager
    {
    public:
        void fetchObjectResourcesDesc(std::string object_resource_path);
        bool loadObjectResource(int res_index);
        void addObjectResourceToPipeline(int res_index);

        void loadMesh(RenderObjectResource& obj_res);
        void loadMaterialTexture(RenderObjectResource& obj_res);
        uint8_t* loadRawTexture(std::string tex_file, int& w, int&h, int& n);

        void addMeshDataToPipeline(int asset_ind);
        void addMaterialDataToPipeline(int asset_ind);

        void createObjectInstance(int res_index, RenderObjectInstance& obj_instance);
    
    public:
        // object resources
        std::vector<RenderObjectResource> m_object_resource_list;  

    private:
        // directorys
        std::string m_object_resource_dir;

        // assets
        std::vector<MeshAsset> m_mesh_asset_list;
        std::vector<MaterialTexAsset> m_tex_asset_list;

        // loader
        tinyobj::ObjReader obj_reader;
    };
}