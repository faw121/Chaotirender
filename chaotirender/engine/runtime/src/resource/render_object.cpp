#include <runtime/resource/render_object.h>

namespace Chaotirender
{
    RenderObjectInstance::RenderObjectInstance(const RenderObjectResource& resource, std::string name)
    {
        m_name = name;
        m_sub_mesh = resource.m_sub_mesh;
    }
}