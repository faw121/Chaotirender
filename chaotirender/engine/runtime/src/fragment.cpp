#include <runtime/fragment.h>

#include <cassert>

namespace Chaotirender
{
    FragmentBuffer::FragmentBuffer(TriangleBoundingBox bb): 
        bounding_box(bb)
    {
        row = bb.xmax - bb.xmin + 1;
        size = row * (bb.ymax - bb.ymin + 1);

        // default false
        mask.resize(size);
        
        buffer_index.resize(size);
        // set index to -1
        std::fill(buffer_index.begin(), buffer_index.end(), -1);
    }

    void FragmentBuffer::setFragment(int i, int j, Fragment& fragment)
    {   
        int index = row * i + j;

        assert(index >= 0 && index < size);

        mask[index] = true;
        fragments.push_back(fragment);
        buffer_index[index] = fragments.size() - 1;
    }

    void FragmentBuffer::setFragmentColor(int i, int j, glm::vec3 color)
    {
        int index = row * i + j;

        assert(index >= 0 && index < size);

        fragments[buffer_index[index]].color = color;
    }
}