# pragma once

#include <runtime/mesh_data.h>
#include <runtime/texture.h>
#include <runtime/frame_buffer.h>
#include <runtime/buffer_list.h>
#include <runtime/vertex_shader.h>
#include <runtime/pixel_shader.h>

#include <glm/mat4x4.hpp>
#include <tbb/tbb.h>

#include <vector>
#include <memory>

namespace Chaotirender
{
    enum class PrimitiveType
    {
        line,
        triangle
    };

    class RasterizeConfig
    {
    public:
        PrimitiveType primitive {PrimitiveType::triangle};
        Color line_color;
        bool back_face_culling {true};
    };
    
    class ShadingConfig
    {
    public:
        bool early_z {false};
    };

    class RenderConfig
    {
    public:
        bool enable_parallel {true};
        RasterizeConfig rasterize_config;
        ShadingConfig shading_config;
    };

    using VertexBuffer = std::vector<Vertex>;
    using IndexBuffer = std::vector<index_t>;
    using VertexBufferList = BufferList<VertexBuffer>;
    using IndexBufferList = BufferList<IndexBuffer>;
    using TextureBufferList = BufferList<Texture>;

    class RenderPipeline
    {
    public:
        friend class Rasterize;
        friend class ProcessGeometry;

        RenderPipeline(int w, int h);

        buffer_id addVertexBuffer(const std::vector<Vertex>& data);
        buffer_id addVertexBuffer(std::unique_ptr<std::vector<Vertex>>&& data);

        buffer_id addIndexBuffer(const std::vector<index_t>& data);
        buffer_id addIndexBuffer(std::unique_ptr<std::vector<index_t>>&& data);

        buffer_id addTexture(std::unique_ptr<Texture>&& texture);
        buffer_id addTexture(int w, int h, int channels, uint8_t* texels);
        
        void bindVertexBuffer(buffer_id id);
        void bindIndexBuffer(buffer_id id);

        void bindVertexShaderTexture(buffer_id id, std::string name, SampleType sample_type=SampleType::NEAREST);
        void bindPixelShaderTexture(buffer_id id, std::string name, SampleType sample_type=SampleType::NEAREST);

        void removeVertexBuffer(buffer_id id);
        void removeIndexBuffer(buffer_id id);
        void removeTexture(buffer_id id);

        void clearVertexBuffer();
        void clearIndexBuffer();
        void clearTexture();

        void setVertexShader(std::shared_ptr<VertexShader> shader);
        void setPixelShader(std::shared_ptr<PixelShader> shader);

        void draw();

    public:
        RenderConfig render_config;

        FrameBuffer frame_buffer;

    private:
        void drawTriangleSerial();
        void drawWireframeSerial();
        void drawTriangleParallel();
        void drawWireframeParallel();

    private:
        VertexBufferList  m_vertex_buffer_list;
        IndexBufferList   m_index_buffer_list;
        TextureBufferList m_texture_buffer_list;

        VertexBuffer* m_vertex_buffer {nullptr};
        IndexBuffer*  m_index_buffer {nullptr};

        std::shared_ptr<VertexShader> m_vertex_shader;
        std::shared_ptr<PixelShader>  m_pixel_shader;

        int m_w;
        int m_h;
        glm::mat4 m_screen_mapping_matrix;
    };

    extern RenderPipeline g_render_pipeline;
}