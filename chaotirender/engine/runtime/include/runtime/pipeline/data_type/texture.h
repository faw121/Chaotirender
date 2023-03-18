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
        SampleType m_sample_type = SampleType::NEAREST;

        virtual glm::vec4 sample(float u, float v) = 0;
        virtual ~TextureBase() {}
    };

    class Texture: public TextureBase
    {
    public:
        Texture() = default;

        Texture(uint32_t w, uint32_t h, uint32_t channel, uint8_t* texels);

        bool isValid() const { return m_texels != nullptr; }

        glm::vec4 sample(float u, float v) override;

        glm::vec4 sampleNearest(float u, float v);

        glm::vec4 sampleBilinear(float u, float v);

        glm::vec4 getTexel(int i, int j);

    public:
        uint32_t m_width {0};
        uint32_t m_height {0};
        uint32_t m_channel {0};
        uint32_t m_depth {0};
        uint32_t m_mip_level {0};
        uint8_t* m_texels {nullptr}; // origin at top-left

        ~Texture()
        {
            if (m_texels)
                free(m_texels);
        }
    };
}