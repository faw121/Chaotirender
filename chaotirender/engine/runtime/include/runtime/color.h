#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Chaotirender
{   
    using Color4f = glm::vec4;
    using Color3f = glm::vec3;
    using Color4i = glm::i8vec4;
    using Color3i = glm::i8vec3;

    struct Color
    {
        uint8_t r {};
        uint8_t g {};
        uint8_t b {};
        uint8_t a {255};

        Color() = default;
        Color(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_ = 255): r(r_), g(g_), b(b_), a(a_) {}
    };
}