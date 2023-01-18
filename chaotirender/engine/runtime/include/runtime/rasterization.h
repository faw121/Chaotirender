#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <runtime/render_pipeline_global_context.h>
#include <runtime/fragment.h>

namespace Chaotirender
{   
    struct TriangleBoundingBox
    {
        int xmin;
        int ymin;
        int xmax;
        int ymax;
    };

    typedef glm::vec3 EdgeEquation;

    extern EdgeEquation e0;
    extern EdgeEquation e1;
    extern EdgeEquation e2;

    extern float s0;
    extern float s1;
    extern float s2;

    inline float evaluateEdgeEquation(float x, float y, EdgeEquation e) { return x * e.x + y * e.y + e.z; }

    void rasterizeLine(float x0_, float y0_, float x1_, float y1_);

    // TODO: 正式地记一下 perspective correct interpolation
    // review quaternion
    // 1. clipping: near and far, [-1, 1] homogeneous space
    // triangle -> no band, line -> screen
    // 2. shader: vertex shader 暂时不知道几何处理要做啥
    // - 光照位置这种 用vector of attributes, 请求-分配id，自己用
    // 3. 用网格贴图测试插值是否正确
    /**
     * geometry -> 4d vectors, buffer needed, rasterization shouldn't use vertex buffer
     * division done in rasterization
    */
    void rasterizeTriangle();

    void triangleSetup(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2);

    bool insideTriangle(float x, float y);

    void triangleBoundingBox(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, TriangleBoundingBox& bb);

    void swapi(int& x, int& y);
}