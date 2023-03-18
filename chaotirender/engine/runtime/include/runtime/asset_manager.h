#pragma once

#include <tiny_obj_loader.h>

#include <runtime/pipeline/data_type/pipeline_data_type.h>

#include <filesystem>

namespace Chaotirender
{   
    // TODO: should manage resources under folder /asset 
    class AssetManagerOld
    {
    public:
        bool loadAsset(std::string asset_path);

        uint8_t* loadRawTexture(std::string file, int& w, int&h, int& n);

        // void getObjectResource(std::string asset_path, RenderObjectResource& resource);

    // private:
        // void getMeshData(RenderObjectResource& object_resource);
        // void getTextureData(RenderObjectResource& object_resource);
        
    private:
        tinyobj::ObjReader obj_reader;
        std::filesystem::path m_asset_folder;
    };

    bool loadAsset(std::string asset_path);

    bool getDrawData(std::vector<Vertex>& vertex_list, std::vector<index_t>& index_list);

    // bool getMeshData(std::vector<RawMesh>& meshes);
}