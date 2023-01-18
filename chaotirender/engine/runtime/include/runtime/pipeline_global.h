#pragma once

#include <runtime/tgaimage.h>
#include <runtime/mesh_data.h>
#include <runtime/fragment.h>
#include <glm/mat4x4.hpp>

namespace Chaotirender
{
    extern const int screen_width;
    extern const int screen_height;

    extern TGAImage color_buffer;
    extern TGAColor white;

    // render pipeline buffer, face only for temporary
    // TODO: how to deal with different primitives?
    extern std::vector<Vertex>  vertex_buffer;
    extern std::vector<index_t> index_buffer; // 似乎不需要？读的时候就按顺序放了
    
    extern std::vector<Vertex> geometry_vertex_buffer;
    extern std::vector<index_t> geometry_index_buffer;

    extern std::vector<FragmentBuffer*> fragment_buffers;

    extern TGAColor draw_color;
}