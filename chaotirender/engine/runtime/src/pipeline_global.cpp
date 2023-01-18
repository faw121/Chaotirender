#include <runtime/pipeline_global.h>

namespace Chaotirender
{
    const int screen_width  = 800;
    const int screen_height = 800;

    TGAImage color_buffer(screen_width, screen_height, TGAImage::RGB);
    TGAColor white = TGAColor(255, 255, 255, 255);

    // render pipeline buffer, face only for temporary
    // TODO: how to deal with different primitives?
    std::vector<Vertex>  vertex_buffer;
    std::vector<index_t> index_buffer; // 似乎不需要？读的时候就按顺序放了

    std::vector<Vertex> geometry_vertex_buffer;
    std::vector<index_t> geometry_index_buffer;

    std::vector<FragmentBuffer*> fragment_buffers;

    TGAColor draw_color;
}