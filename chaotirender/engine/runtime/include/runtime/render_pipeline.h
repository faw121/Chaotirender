# pragma once

#include <runtime/mesh_data.h>

namespace Chaotirender
{
    enum class PrimitiveType
    {
        line,
        triangle
    };

    void runPipeline();

    void testPipeline();
}