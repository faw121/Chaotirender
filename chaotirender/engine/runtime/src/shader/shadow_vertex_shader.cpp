#include <runtime/shader/shadow_vertex_shader.h>

#include <glm/gtc/matrix_inverse.hpp>

namespace Chaotirender
{   
    ShadowVertexShader::ShadowVertexShader(): model_matrix(1), view_matrix(1), projection_matrix(1) {}

    void ShadowVertexShader::shadeVertex(Vertex& v) 
    {
        glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;
        
        v.position_homo = mvp * v.position_homo;
    }
}
