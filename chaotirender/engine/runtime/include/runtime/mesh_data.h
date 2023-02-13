#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <vector>
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

    struct MeshData
    {
        std::vector<Vertex> vertex_buffer;
        std::vector<int>    index_buffer;
    };

    struct Triangle
    {
        Vertex m_v0;
        Vertex m_v1;
        Vertex m_v2;

        Triangle(Vertex& v0, Vertex& v1, Vertex& v2): m_v0(v0), m_v1(v1), m_v2(v2) {}
        Triangle() = default;
    };

    struct TriangleGroup
    {
        std::vector<Triangle> triangles;
    };
}