#include <runtime/simple_vertex_shader.h>

#include <glm/gtc/matrix_inverse.hpp>

namespace Chaotirender
{   
    SimpleVertexShader::SimpleVertexShader()
    {
        model_matrix      = glm::mat4(1);
        view_matrix       = glm::mat4(1);
        projection_matrix = glm::mat4(1);
    }

    void SimpleVertexShader::shadeVertex(Vertex& v)
    {
        glm::mat4 mv = view_matrix * model_matrix;
        glm::mat4 mvp = projection_matrix * mv;
        
        // compute light in world space 
        glm::mat3 normal_matrix = glm::inverseTranspose(mv);

        v.position_homo = mvp * v.position_homo;
        v.normal = normal_matrix * v.normal;
    }
}
