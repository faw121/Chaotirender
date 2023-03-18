#include <runtime/pipeline/render_pipeline.h>

#include <runtime/pipeline/process_geometry.h>
#include <runtime/pipeline/rasterize.h>

#include <runtime/debug.h>
#include <runtime/tick_tock.h>

namespace Chaotirender
{   
    RenderPipeline g_render_pipeline(800, 800);

    RenderPipeline::RenderPipeline(int w, int h): frame_buffer(w, h), m_w(w), m_h(h)
    {
        setScreenMatrix(w, h);
    }

    void RenderPipeline::setScreenMatrix(int w, int h)
    {
        m_screen_mapping_matrix[0][0] = w / 2;
        m_screen_mapping_matrix[3][0] = w / 2;
        m_screen_mapping_matrix[1][1] = h / 2;
        m_screen_mapping_matrix[3][1] = h / 2;
        m_screen_mapping_matrix[2][2] = 1;
        m_screen_mapping_matrix[3][3] = 1;
    }

    void RenderPipeline::setViewPort(int w, int h)
    {
        m_w = w;
        m_h = h;
        setScreenMatrix(w, h);
        frame_buffer.resizeAndClear(w, h);
    }

    void RenderPipeline::draw()
    {   
        if (render_config.rasterize_config.primitive == PrimitiveType::triangle)
        {
            if (render_config.enable_parallel)
                drawTriangleParallel();
            else
                drawTriangleSerial();
        }
        else
        {
            if (render_config.enable_parallel)
                drawWireframeParallel();
            else
                drawWireframeSerial();
        }     
    }

    void RenderPipeline::drawTriangleParallel()
    {   
        // assemble triangle
        std::vector<Triangle> triangle_list;
        VertexBuffer& vertex_buffer = *m_vertex_buffer;
        IndexBuffer& index_buffer = *m_index_buffer;

        int num_faces = index_buffer.size() / 3;

        for (int f = 0; f < num_faces; f++)
        {
            int index_base = 3 * f;
            Triangle t(vertex_buffer[index_buffer[index_base + 0]], vertex_buffer[index_buffer[index_base + 1]], vertex_buffer[index_buffer[index_base + 2]]);

            t.m_v0.position_homo = glm::vec4(t.m_v0.position, 1);
            t.m_v1.position_homo = glm::vec4(t.m_v1.position, 1);
            t.m_v2.position_homo = glm::vec4(t.m_v2.position, 1);

            triangle_list.push_back(t);
        }

        tbb::parallel_for(tbb::blocked_range<size_t>(0, triangle_list.size()),
        [&](tbb::blocked_range<size_t> r) -> void
        {   
            for(size_t i = r.begin(); i < r.end(); i++)
            {
                auto t = triangle_list[i];
                ProcessGeometry process_geometry;

                // process geometry
                TriangleGroup triangle_group;
                process_geometry(t, triangle_group);

                // rasterize and draw
                for (auto& triangle: triangle_group.triangles)
                {   
                    Rasterize rasterize(g_render_pipeline.m_w, g_render_pipeline.m_h);
                    rasterize(triangle);
                }
            }
        });
    }

    void RenderPipeline::drawTriangleSerial()
    {
        VertexBuffer& vertex_buffer = *(m_vertex_buffer);
        IndexBuffer& index_buffer = *(m_index_buffer);

        int num_faces = m_index_buffer->size() / 3;

        for (int f = 0; f < num_faces; f++)
        {
            int index_base = 3 * f;
            Triangle t(vertex_buffer[index_buffer[index_base + 0]], vertex_buffer[index_buffer[index_base + 1]], vertex_buffer[index_buffer[index_base + 2]]);

            t.m_v0.position_homo = glm::vec4(t.m_v0.position, 1);
            t.m_v1.position_homo = glm::vec4(t.m_v1.position, 1);
            t.m_v2.position_homo = glm::vec4(t.m_v2.position, 1);

            TriangleGroup triangle_group;
            ProcessGeometry process_geometry;
            process_geometry(t, triangle_group);

            // rasterize and draw
            for (auto& triangle: triangle_group.triangles)
            {   
                Rasterize rasterize(g_render_pipeline.m_w, g_render_pipeline.m_h);
                rasterize(triangle);
            }
        }
    }

    void RenderPipeline::drawWireframeSerial()
    {
        VertexBuffer& vertex_buffer = *(m_vertex_buffer);
        IndexBuffer& index_buffer = *(m_index_buffer);

        int num_faces = m_index_buffer->size() / 3;

        for (int f = 0; f < num_faces; f++)
        {
            int index_base = 3 * f;
            Triangle t(vertex_buffer[index_buffer[index_base + 0]], vertex_buffer[index_buffer[index_base + 1]], vertex_buffer[index_buffer[index_base + 2]]);

            t.m_v0.position_homo = glm::vec4(t.m_v0.position, 1);
            t.m_v1.position_homo = glm::vec4(t.m_v1.position, 1);
            t.m_v2.position_homo = glm::vec4(t.m_v2.position, 1);

            LineGroup line_group;
            ProcessGeometry process_geometry;
            process_geometry(t, line_group);

            // rasterize and draw
            for (auto& line: line_group.lines)
            {   
                Rasterize rasterize(g_render_pipeline.m_w, g_render_pipeline.m_h);
                rasterize(line, render_config.rasterize_config.line_color);
            }
        }
    }

    void RenderPipeline::drawWireframeParallel()
    {   
        // assemble triangle
        std::vector<Triangle> triangle_list;
        VertexBuffer& vertex_buffer = *m_vertex_buffer;
        IndexBuffer& index_buffer = *m_index_buffer;

        int num_faces = m_index_buffer->size() / 3;

        for (int f = 0; f < num_faces; f++)
        {
            int index_base = 3 * f;
            Triangle t(vertex_buffer[index_buffer[index_base + 0]], vertex_buffer[index_buffer[index_base + 1]], vertex_buffer[index_buffer[index_base + 2]]);

            t.m_v0.position_homo = glm::vec4(t.m_v0.position, 1);
            t.m_v1.position_homo = glm::vec4(t.m_v1.position, 1);
            t.m_v2.position_homo = glm::vec4(t.m_v2.position, 1);

            triangle_list.push_back(t);
        }

        tbb::parallel_for(tbb::blocked_range<size_t>(0, triangle_list.size()),
        [&](tbb::blocked_range<size_t> r) -> void
        {   
            for(size_t i = r.begin(); i < r.end(); i++)
            {
                auto t = triangle_list[i];
                ProcessGeometry process_geometry;

                // process geometry
                LineGroup line_group;
                process_geometry(t, line_group);

                // rasterize and draw
                for (auto& line: line_group.lines)
                {   
                    Rasterize rasterize(g_render_pipeline.m_w, g_render_pipeline.m_h);
                    rasterize(line, render_config.rasterize_config.line_color);
                }
            }
        });
    }
    
    buffer_id RenderPipeline::addVertexBuffer(const std::vector<Vertex>& data) { return m_vertex_buffer_list.add(data); }
    buffer_id RenderPipeline::addVertexBuffer(std::unique_ptr<std::vector<Vertex>>&& data) {  return m_vertex_buffer_list.add(std::move(data)); }

    buffer_id RenderPipeline::addIndexBuffer(const std::vector<index_t>& data) { return m_index_buffer_list.add(data); }
    buffer_id RenderPipeline::addIndexBuffer(std::unique_ptr<std::vector<index_t>>&& data) { return m_index_buffer_list.add(std::move(data)); }

    buffer_id RenderPipeline::addTexture(std::unique_ptr<Texture>&& texture) { return m_texture_buffer_list.add(std::move(texture)); }
    buffer_id RenderPipeline::addTexture(int w, int h, int channels, uint8_t* texels) { return m_texture_buffer_list.add(std::make_unique<Texture>(w, h, channels, texels)); }

    void RenderPipeline::bindVertexBuffer(buffer_id id)
    {
        m_vertex_buffer = m_vertex_buffer_list.get(id);
    }

    void RenderPipeline::bindIndexBuffer(buffer_id id)
    {
        m_index_buffer = m_index_buffer_list.get(id);
    }

    void RenderPipeline::removeVertexBuffer(buffer_id id)
    {
        if (m_vertex_buffer == m_vertex_buffer_list.get(id))
            m_vertex_buffer = nullptr;
        m_vertex_buffer_list.remove(id);
    }

    void RenderPipeline::removeIndexBuffer(buffer_id id)
    {
        if (m_index_buffer == m_index_buffer_list.get(id))
            m_index_buffer = nullptr;
        m_index_buffer_list.remove(id);
    }

    void RenderPipeline::clearVertexBuffer()
    {
        m_vertex_buffer = nullptr;
        m_vertex_buffer_list.clear();
    }

    void RenderPipeline::clearIndexBuffer()
    {
        m_index_buffer = nullptr;
        m_index_buffer_list.clear();
    }

    void RenderPipeline::setVertexShader(std::shared_ptr<VertexShader> shader) { m_vertex_shader = shader; }

    void RenderPipeline::setPixelShader(std::shared_ptr<PixelShader> shader) { m_pixel_shader = shader; }

    void RenderPipeline::bindVertexShaderTexture(buffer_id id, std::string name, SampleType sample_type) 
    { 
        auto texture = m_texture_buffer_list.get(id);
        texture->m_sample_type = sample_type; 
        m_vertex_shader->texture_list[name] = texture;
    }

    void RenderPipeline::bindPixelShaderTexture(buffer_id id, std::string name, SampleType sample_type)
    {   
        auto texture = m_texture_buffer_list.get(id);
        texture->m_sample_type = sample_type; 
        m_pixel_shader->texture_list[name] = texture; 
    }

    void RenderPipeline::removeTexture(buffer_id id)
    {
        auto ptr = m_texture_buffer_list.get(id);

        for (auto& pair: m_vertex_shader->texture_list)
            if (pair.second == ptr)
                pair.second = nullptr;

        for (auto& pair: m_pixel_shader->texture_list)
            if (pair.second == ptr)
                pair.second = nullptr;  
        m_texture_buffer_list.remove(id);
    }

    void RenderPipeline::clearTexture()
    {
        for (auto& pair: m_vertex_shader->texture_list)
            pair.second = nullptr;

        for (auto& pair: m_pixel_shader->texture_list)
            pair.second = nullptr;
        m_texture_buffer_list.clear();
    }
}