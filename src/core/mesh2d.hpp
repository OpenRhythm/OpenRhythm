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

    std::vector<Vertex2D> create_rect_mesh(glm::vec4 color);

    class Render2D
    {
    public:
        Render2D(ShaderProgram *program, Texture *texture);
        void init_gl();
        void mesh_clear();
        void add_mesh(const Mesh2D& mesh);
        void mesh_commit();
        void render();
        ~Render2D();

    private:
        ShaderProgram *m_program;
        Texture *m_texture;
        BufferTexture m_matTexBuffer;
        BufferTexture m_matTexIndexBuffer;
        GLuint m_vertLoc;
        GLuint m_uvLoc;
        GLuint m_colorLoc;
        GLuint m_texSampID;
        GLuint m_matBufTexID;
        GLuint m_matIndexBufTexID;

        GLuint m_vbo;
        GLuint m_matBufferObject;
        GLuint m_matIndexBufferObject;

        std::vector<unsigned int> m_meshMatrixIndex;

        std::vector<Vertex2D> m_vertices;
        std::vector<glm::mat4> m_matrices;
    };
} // namespace ORCore
