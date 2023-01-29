#pragma once

#include <runtime/rasterizer.h>
#include <runtime/debug.h>

namespace Chaotirender
{
    void testInterpolation()
    {
        Rasterizer rst;

        glm::vec2 p0(0, 4);
        glm::vec2 p1(-4, 0);
        glm::vec2 p2(3, 2);

        glm::vec2 p = 0.9f * p0 + 0.05f * p1 + 0.05f * p2;

        EdgeEquation e0, e1, e2;
        float s0, s1, s2;

        rst.triangleSetup(p0, p1, p2, e0, e1, e2);
        
        std::cout << "x, y: " << p.x << ',' << p.y << std::endl;
        if (rst.insideTriangle(p.x, p.y, s0, s1, s2, e0, e1, e2))
        {
            float alpha, beta, gamma, s;
            s = s0 + s1 + s2;
            alpha = s0 / s;
            beta = s1 / s;
            gamma = 1 - alpha - beta;
            gamma = s2 / s;

            std::cout << "a, b, c: " << alpha << ',' << beta << ',' << gamma << ',' << std::endl;
        }

    }
}