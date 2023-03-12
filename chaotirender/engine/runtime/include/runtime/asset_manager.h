#pragma once

#include <tiny_obj_loader.h>

#include <runtime/mesh_data.h>
#include <runtime/render_object.h>

namespace Chaotirender
{   
    // TODO: should manage resources under folder /asset 
    class AssetManager
    {
    public:
        bool loadAsset(std::string asset_path);

        uint8_t* loadRawTexture(std::string file, int& w, int&h, int& n);

        RenderObjectResource getObjectResource(std::string asset_path);

    private:
        void getMeshData(RenderObjectResource& object_resource);
        void getTextureData(RenderObjectResource& object_resource);
        
    private:
        tinyobj::ObjReader obj_reader;
    };

    bool loadAsset(std::string asset_path);

    bool getDrawData(std::vector<Vertex>& vertex_list, std::vector<index_t>& index_list);

    bool getMeshData(std::vector<RawMesh>& meshes);
}