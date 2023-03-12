#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <vector>
#include <memory>
#include <map>
#include <any>
#include <string>

namespace Chaotirender
{
    // use vec, easy to distinguish data for pipeline implementation
    class Vertex
    {  
    public: 
        union 
        {
            glm::vec3 position;
            glm::vec4 position_homo;
        };

        glm::vec3 normal;

        glm::vec2 uv;

        glm::vec3 world_position;
    
    public:
        Vertex(float px, float py, float pz, float nx, float ny, float nz, float u, float v):
            position(px, py, pz), normal(nx, ny, nz), uv(u, v) {}

        Vertex(float px, float py, float pz): 
            position(px, py, pz), normal(-1, -1, -1), uv(-1, -1) {}

        Vertex(float px, float py, float pz, float pw, float nx, float ny, float nz, float u, float v):
            position_homo(px, py, pz, pw), normal(nx, ny, nz), uv(u, v) {}
        
        Vertex() = default;
    };

    class PipelineVertex: public Vertex
    {
    public:
        PipelineVertex(Vertex& v): Vertex(v) {}

        PipelineVertex() = default;

    public:
        std::map<std::string, std::any> varyings;
    };

    typedef unsigned int index_t;

    using VertexBuffer = std::vector<Vertex>;
    using IndexBuffer = std::vector<index_t>;

    struct Triangle
    {
        Vertex m_v0;
        Vertex m_v1;
        Vertex m_v2;

        Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2): m_v0(v0), m_v1(v1), m_v2(v2) {}
        Triangle() = default;
    };

    struct TriangleGroup
    {
        std::vector<Triangle> triangles;
    };

    struct Line
    {
        glm::vec4 m_p0;
        glm::vec4 m_p1;

        Line(glm::vec4 p0, glm::vec4 p1): m_p0(p0), m_p1(p1) {}
    };

    struct LineGroup
    {
        std::vector<Line> lines;
    };

    struct RawTexture
    {
        std::string tex_file_name; // relative path to resource folder

        int width;
        int height;
        int channels;
        uint8_t* texels {nullptr};
    };

    struct RawMaterial
    {
        std::string name;

        glm::vec3 ka {0.f, 0.f, 0.f};
        glm::vec3 kd {0.f, 0.f, 0.f};
        glm::vec3 ks {0.f, 0.f, 0.f};
        float shininess {0.f};

        // TODO: not sure about material model, phong? metallic what what?
        RawTexture ambient_texture;
        RawTexture diffuse_texture;
        RawTexture specular_texture;
        RawTexture normal_texture;
    };

    using buffer_id = int;

    struct RawMesh
    {
        RawMaterial material;

        buffer_id vertex_buffer_id {-1};
        buffer_id index_buffer_id {-1};

        buffer_id ambient_tex_id {-1};
        buffer_id diffuse_tex_id {-1};
        buffer_id specular_tex_id {-1};
        buffer_id normal_tex_id {-1};

        VertexBuffer vertex_buffer;
        IndexBuffer  index_buffer;
    };
}