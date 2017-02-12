#include "mesh2d.hpp"
#include <iostream>

namespace ORCore
{
    std::vector<Vertex2D> create_rect_mesh()
    {
        return {
            // Vertex2     UV            Color
            {{0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}},
            {{0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f}},
            {{1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}},
            {{0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f}},
            {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f}},
            {{1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}}
        };
    }


    Render2D::Render2D(ShaderProgram *program, Texture *texture)
    : m_program(program), m_texture(texture), m_matTexBuffer(GL_RGBA32F), m_matTexIndexBuffer(GL_R32UI)
    {
        m_vertices.reserve(32*6); // 32 object each object has 3 verts of 2 values
        m_matrices.reserve(32);
        m_meshMatrixIndex.reserve(32*2); // 32 objects each object has 2 triangles
        init_gl();
    }

    void Render2D::init_gl()
    {
        glEnable(GL_BLEND);
        m_vertLoc = m_program->vertex_attribute("position");
        m_uvLoc = m_program->vertex_attribute("vertexUV");
        m_colorLoc = m_program->vertex_attribute("color");
        m_texSampID = m_program->uniform_attribute("textureSampler");
        m_matBufTexID = m_program->uniform_attribute("matrixBuffer");
        m_matIndexBufTexID = m_program->uniform_attribute("matrixIndices");
        
        // Vertex Buffer
        glGenBuffers(1, &m_vbo);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glEnableVertexAttribArray(m_vertLoc);
        glEnableVertexAttribArray(m_uvLoc);
        glEnableVertexAttribArray(m_colorLoc);

        glVertexAttribPointer( m_vertLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), reinterpret_cast<void *>(offsetof(Vertex2D, vertex)));
        glVertexAttribPointer( m_uvLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), reinterpret_cast<void *>(offsetof(Vertex2D, uv)));
        glVertexAttribPointer( m_colorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), reinterpret_cast<void *>(offsetof(Vertex2D, color)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Matrix Texture Buffer Objects
        glGenBuffers(1, &m_matBufferObject);
        glBindBuffer(GL_TEXTURE_BUFFER, m_matBufferObject);

        glGenBuffers(1, &m_matIndexBufferObject);
        glBindBuffer(GL_TEXTURE_BUFFER, m_matIndexBufferObject);

        glBindBuffer(GL_TEXTURE_BUFFER, 0);

    }

    void Render2D::add_mesh(const Mesh2D& mesh)
    {
        int meshVertexCount = mesh.vertices.size();

        // Add one index per vertex
        for (int i = 0; i < meshVertexCount/3; i++) // 3 is for 3 vertices in a triangle
        {
            m_meshMatrixIndex.push_back(m_matrices.size());
        }
        m_matrices.push_back(glm::scale(glm::translate(glm::mat4(1.0f), mesh.translate), mesh.scale));
        m_vertices.insert(m_vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
}

    // update buffer objects
    void Render2D::mesh_commit()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(Vertex2D), &m_vertices[0], GL_DYNAMIC_DRAW);

        glBindBuffer(GL_TEXTURE_BUFFER, m_matBufferObject);
        glBufferData(GL_TEXTURE_BUFFER, m_matrices.size()*sizeof(glm::mat4), &m_matrices[0], GL_DYNAMIC_DRAW);
        m_matTexBuffer.assign_buffer(m_matBufferObject);

        glBindBuffer(GL_TEXTURE_BUFFER, m_matIndexBufferObject);
        glBufferData(GL_TEXTURE_BUFFER, m_meshMatrixIndex.size()*sizeof(unsigned int), &m_meshMatrixIndex[0], GL_DYNAMIC_DRAW);
        m_matTexIndexBuffer.assign_buffer(m_matIndexBufferObject);

        glBindBuffer(GL_TEXTURE_BUFFER, 0);

    }

    void Render2D::update()
    {
        // m_vertices.clear();
        // m_matrices.clear();
        // m_meshMatrixIndex.clear();
    }

    void Render2D::render()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        // Bind textures
        m_texture->bind(m_texSampID);
        m_matTexBuffer.bind(m_matBufTexID);
        m_matTexIndexBuffer.bind(m_matIndexBufTexID);

        glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());

    }

    Render2D::~Render2D()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(m_colorLoc);
        glDisableVertexAttribArray(m_uvLoc);
        glDisableVertexAttribArray(m_vertLoc);

        glDeleteBuffers(1, &m_vbo);
    }

} // namespace ORCore
