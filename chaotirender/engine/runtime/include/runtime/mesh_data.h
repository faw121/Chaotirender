#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>

namespace Chaotirender
{
    // use vec, easy to distinguish data for pipeline implementation
    struct Vertex
    {   
        union 
        {
            glm::vec3 position;
            glm::vec4 position_homo;
        };
        glm::vec3 normal;
        glm::vec2 uv;

        Vertex(float px, float py, float pz, float nx, float ny, float nz, float u, float v):
            position(px, py, pz), normal(nx, ny, nz), uv(u, v) {}

        Vertex(float px, float py, float pz): 
            position(px, py, pz), normal(-1, -1, -1), uv(-1, -1) {}

        Vertex(float px, float py, float pz, float pw, float nx, float ny, float nz, float u, float v):
            position_homo(px, py, pz, pw), normal(nx, ny, nz), uv(u, v) {}

        // Vertex(const Vertex& v): position_homo(glm::vec4(v.position, 1)), normal(glm::vec3(v.normal)), uv(glm::vec2(v.uv)) {}
        
        Vertex() = default;
    };

    typedef unsigned int index_t;

    struct MeshData
    {
        std::vector<Vertex> vertex_buffer;
        std::vector<int>    index_buffer;
    };
}