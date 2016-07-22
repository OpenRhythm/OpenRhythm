#include <glad/glad.h>
#include "shader.hpp"
#include "texture.hpp"
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

namespace MgCore
{
    class Mesh2D
    {
    private:
        ShaderProgram *m_program;
        Texture *m_texture;
        GLuint m_vbo;
        GLuint m_uvbo;
        GLuint m_vertLoc;
        GLuint m_modelAttr;
        GLuint m_uvLoc;
        glm::mat4 m_modelMatrix;
        glm::vec3 m_vecTrans;
        glm::vec3 m_vecScale;
        float m_xScale;
        float m_yScale;
        float m_xPos;
        float m_yPos;
        GLfloat m_vertData[8];

    public:
        Mesh2D(ShaderProgram *program, Texture *texture);
        ~Mesh2D();
        void render();
        void update();
        void init_gl();
        void scale(float x, float y);
        void translate(float x, float y);
    };
}
