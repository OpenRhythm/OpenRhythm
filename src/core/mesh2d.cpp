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
    : m_program(program), m_texture(texture)
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
        m_modelAttr = m_program->uniform_attribute("models");
        m_modelIndicesAttr = m_program->uniform_attribute("modelIndices");
        glGenBuffers(1, &m_vbo);

        m_texture->bind();

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glEnableVertexAttribArray(m_vertLoc);
        glEnableVertexAttribArray(m_uvLoc);
        glEnableVertexAttribArray(m_colorLoc);

        glVertexAttribPointer( m_vertLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), reinterpret_cast<void *>(offsetof(Vertex2D, vertex)));
        glVertexAttribPointer( m_uvLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), reinterpret_cast<void *>(offsetof(Vertex2D, uv)));
        glVertexAttribPointer( m_colorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), reinterpret_cast<void *>(offsetof(Vertex2D, color)));
    }

    void Render2D::add_mesh(const Mesh2D& mesh)
    {
        int meshVertexCount = mesh.vertices.size();
        // if we have hit the max batch size ignore the mesh, later on we will support multiple batches.
        if ((m_matrices.size() + meshVertexCount) <= 64)
        {
            // Add one index per vertex
            for (int i = 0; i < meshVertexCount/3;i++)
            {
                m_meshMatrixIndex.push_back(m_matrices.size());
            }
            m_matrices.push_back(glm::scale(glm::translate(glm::mat4(1.0f), mesh.translate), mesh.scale));
            m_vertices.insert(m_vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
        }
    }

    // update buffer objects
    void Render2D::mesh_commit()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(Vertex2D), &m_vertices[0], GL_DYNAMIC_DRAW);
    }

    void Render2D::update()
    {
        // m_vertices.clear();
        // m_matrices.clear();
        // m_meshMatrixIndex.clear();
    }

    void Render2D::render()
    {
        glUniformMatrix4fv(m_modelAttr, m_matrices.size(), GL_FALSE,  glm::value_ptr(m_matrices[0]));
        glUniform1uiv(m_modelIndicesAttr, m_meshMatrixIndex.size(), &m_meshMatrixIndex[0]);

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
