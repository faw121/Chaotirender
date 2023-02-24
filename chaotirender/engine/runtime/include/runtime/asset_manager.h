#pragma once

#include <tiny_obj_loader.h>
#include <runtime/mesh_data.h>

namespace Chaotirender
{   
    bool loadAsset(std::string asset_path);

    bool getDrawData(std::vector<Vertex>& vertex_list, std::vector<index_t>& index_list);
}