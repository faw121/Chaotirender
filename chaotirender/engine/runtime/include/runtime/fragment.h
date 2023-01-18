#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>

namespace Chaotirender
{
    struct Fragment
    {   
        glm::i32vec2 screen_coord;
        float        depth;
        glm::vec3    normal;
        glm::vec2    uv;
        glm::vec3    color;
    };

    struct TriangleBoundingBox
    {
        int xmin;
        int ymin;
        int xmax;
        int ymax;
    };

    class FragmentBuffer
    {
    public:
        FragmentBuffer(TriangleBoundingBox bb);

        void setFragment(int i, int j, Fragment& fragment);

        void setFragmentColor(int i, int j, glm::vec3 color);

        int size;
        int row; // store by row
        TriangleBoundingBox bounding_box;

    public:
        std::vector<bool>     mask;
        std::vector<int>      buffer_index;
        std::vector<Fragment> fragment_buffer;
    };
}


