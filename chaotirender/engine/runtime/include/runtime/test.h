#pragma once

#include <runtime/pipeline/render_pipeline.h>
#include <runtime/render_resource.h>
#include <runtime/shader/simple_vertex_shader.h>
#include <runtime/shader/simple_pixel_shader.h>
#include <runtime/shader/texture_pixel_shader.h>

#include <runtime/resource/asset_manager.h>

#include <runtime/util/tgaimage.h>
#include <runtime/debug.h>
#include <runtime/tick_tock.h>

#include <tbb/tbb.h>

namespace Chaotirender
{  
    void testAssetManager()
    {
        AssetManager asset_manager;

        std::string object_resource_path("asset/");

        asset_manager.fetchObjectResourcesDesc(object_resource_path);

        for (int i = 0; i < 6; i++)
            asset_manager.loadObjectResource(i);
    }
}