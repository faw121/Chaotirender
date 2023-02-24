#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <runtime/fragment.h>
#include <runtime/mesh_data.h>
#include <runtime/render_pipeline.h>

#include <vector>
#include <memory>

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

    typedef glm::i8vec2 TileStep;

    class Rasterize
    {
    public:
        Rasterize(int w, int h);

        void operator()(Triangle& triangle);
        
        void operator()(Triangle& triangle, Color color);

    private:
        float evaluateEdgeEquation(float x, float y, EdgeEquation& e);

        void triangleBoundingBox(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);

        void swapi(int& x, int& y);

        void drawPixel(Fragment& fragment);

        void drawPixelEarlyZ(Fragment& fragment);

        void rasterizeLine(float x0_, float y0_, float x1_, float y1_, Color color);

        bool triangleSetup(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2);

        bool triangleSetupCull(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2);

        void barycentric();

        void triangleSetupTile(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2);

        bool insideTriangle(float x, float y);

    private:
        int m_w;
        int m_h;

        EdgeEquation m_e0;
        EdgeEquation m_e1;
        EdgeEquation m_e2;

        TileStep m_ts0;
        TileStep m_ts1;
        TileStep m_ts2;

        float m_s0;
        float m_s1;
        float m_s2;

        float m_w0;
        float m_w1;
        float m_w2;

        float m_w_inverse;
        float m_alphaw;
        float m_betaw;
        float m_gammaw;

        TriangleBoundingBox m_bounding_box;

        bool (Rasterize::*triangleSetup_ptr)(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2);

        void (Rasterize::*drawPixel_ptr)(Fragment& fragment);
    };
}