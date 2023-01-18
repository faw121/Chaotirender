#pragma once

#include <tiny_obj_loader.h>
#include <runtime/mesh_data.h>
#include <runtime/render_pipeline_global_context.h>

namespace Chaotirender
{   

    bool loadAsset(std::string asset_path);

    bool bindPipelineBuffer();
}