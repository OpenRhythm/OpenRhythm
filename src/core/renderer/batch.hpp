#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "texture.hpp"
#include "mesh.hpp"

namespace ORCore
{
    class Batch
    {
    public:
        Batch(ShaderProgram *program, Texture *texture, int batchSize);
        void init_gl();
        void clear();
        bool add_mesh(const Mesh& mesh, glm::mat4& transform);
        void commit();
        void render();
        ~Batch();

    private:
        ShaderProgram *m_program;
        Texture *m_texture;
        int m_batchSize;
        BufferTexture m_matTexBuffer;
        BufferTexture m_matTexIndexBuffer;
        GLuint m_vertLoc;
        GLuint m_uvLoc;
        GLuint m_colorLoc;
        GLuint m_texSampID;
        GLuint m_matBufTexID;
        GLuint m_matIndexBufTexID;

        GLuint m_vao;
        GLuint m_vbo;
        GLuint m_matBufferObject;
        GLuint m_matIndexBufferObject;

        std::vector<unsigned int> m_meshMatrixIndex;

        std::vector<Vertex> m_vertices;
        std::vector<glm::mat4> m_matrices;
    };
} // namespace ORCore
