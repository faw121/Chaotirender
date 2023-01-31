#pragma once

#include <glm/vec4.hpp>

#include <cstdint>
#include <memory>

namespace Chaotirender
{   
    enum class SampleType: uint8_t
    {
        NEAREST = 0,
        BILINEAR
    };

    class TextureBase
    {
    public:
        SampleType sample_type = SampleType::NEAREST;

        virtual glm::vec4 sample(float u, float v) = 0;
        virtual ~TextureBase() {}
    };

    class Texture: public TextureBase
    {
    public:
        bool isValid() const { return texels != nullptr; }

        virtual glm::vec4 sample(float u, float v) override;

        glm::vec4 sampleNearest(float u, float v);

        glm::vec4 sampleBilinear(float u, float v);

        glm::vec4 getTexel(int i, int j);

    public:
        uint32_t width {0};
        uint32_t height {0};
        uint32_t channel {0};
        uint32_t depth {0};
        uint32_t mip_level {0};
        uint8_t* texels {nullptr}; // origin at top-left

        Texture() = default;
        ~Texture()
        {
            if (texels)
                free(texels);
        }
    };
}