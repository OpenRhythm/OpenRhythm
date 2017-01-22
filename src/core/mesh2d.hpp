#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "texture.hpp"

namespace ORCore
{

    struct Mesh2D
    {
        glm::vec3 scale;
        glm::vec3 translate;
        int vertexSize;
        std::vector<GLfloat> vertices;
    };

    std::vector<GLfloat> create_rect_mesh();

    class Render2D
    {
    public:
        Render2D(ShaderProgram *program, Texture *texture);
        void init_gl();
        void update();
        void add_mesh(const Mesh2D& mesh);
        void mesh_commit();
        void render();
        ~Render2D();

    private:
        ShaderProgram *m_program;
        Texture *m_texture;
        GLuint m_vertLoc;
        GLuint m_modelAttr;
        GLuint m_uvLoc;
        GLuint m_matIndAttr;

        GLuint m_vbo;
        GLuint m_uvbo;
        GLuint m_mativbo;

        std::vector<unsigned int> m_meshMatrixIndex;

        std::vector<GLfloat> m_vertices;
        std::vector<glm::mat4> m_matrices;
    };
} // namespace ORCore
