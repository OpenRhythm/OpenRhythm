#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "texture.hpp"

namespace ORCore
{

    struct Vertex2D
    {
        // Use vec here in-case we want to easily transform verts cpu-side.
        glm::vec2 vertex;
        glm::vec2 uv;
        glm::vec4 color;
    };

    struct Mesh2D
    {
        glm::vec3 scale;
        glm::vec3 translate;
        int vertexSize;
        std::vector<Vertex2D> vertices;
    };

    std::vector<Vertex2D> create_rect_mesh();

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
        GLuint m_colorLoc;
        GLuint m_modelIndicesAttr;

        GLuint m_vbo;

        std::vector<unsigned int> m_meshMatrixIndex;

        std::vector<Vertex2D> m_vertices;
        std::vector<glm::mat4> m_matrices;
    };
} // namespace ORCore
