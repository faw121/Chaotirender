#pragma once

#include <runtime/mesh_data.h>
#include <runtime/render_pipeline.h>

#include <vector>
#include <string>

namespace Chaotirender
{   
    class RenderObjectResource
    {
    public:
        void addDataToPipeline();

    public:
        std::string name;
        std::vector<RawMesh> mesh_list;
    };

    struct Material
    {
        // if use tex
        buffer_id ambient_tex_id {-1};
        buffer_id diffuse_tex_id {-1};
        buffer_id specular_tex_id {-1};
        buffer_id normal_tex_id {-1};

        // not use tex, that is id == -1
        glm::vec3 ka {0.f, 0.f, 0.f};
        glm::vec3 kd {0.f, 0.f, 0.f};
        glm::vec3 ks {0.f, 0.f, 0.f};
        float shininess {0.f};
    };

    class Mesh
    {
    public:
        Mesh(const RawMesh& raw_mesh);

        void draw(SampleType tex_sample_type=SampleType::NEAREST);
        
    public:
        buffer_id vertex_buffer_id {-1};
        buffer_id index_buffer_id {-1};

        Material material;

        int num_faces {0};
    };

    struct Transform
    {
        glm::vec3 translation {0.f, 0.f, 0.f};
        glm::vec3 rotation {0.f, 0.f, 0.f}; // not sure, rotate around model axis or world axis?
        glm::vec3 scale {1.f, 1.f, 1.f};
    };

    class RenderObjectInstance
    {
    public:
        RenderObjectInstance(const RenderObjectResource& resource);

    public:
        std::string name;

        // mesh data
        std::vector<Mesh> mesh_list;
        
        // transform
        Transform transform;
        
        // use texture (if has texture)
        bool use_texture {true};
    };
};