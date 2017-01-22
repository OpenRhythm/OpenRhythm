#include "mesh2d.hpp"
#include <iostream>

namespace ORCore
{
    std::vector<GLfloat> create_rect_mesh()
    {
        return {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };
    }

    Render2D::Render2D(ShaderProgram *program, Texture *texture)
    : m_program(program), m_texture(texture)
    {
        init_gl();
    }

    void Render2D::init_gl()
    {
        m_vertLoc = m_program->vertex_attribute("position");
        m_uvLoc = m_program->vertex_attribute("vertexUV");
        m_matIndAttr = m_program->vertex_attribute("matrixIndex");
        m_modelAttr = m_program->uniform_attribute("models");
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_mativbo);
    }

    void Render2D::add_mesh(const Mesh2D& mesh)
    {
        int meshVertexCount = mesh.vertices.size() / mesh.vertexSize;
        // if we have hit the max batch size ignore the mesh, later on we will support multiple batches.
        if ((m_matrices.size() + meshVertexCount) <= 64)
        {
            // Add one index per vertex
            for (int i = 0; i < meshVertexCount;i++)
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
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(GLfloat), &m_vertices[0], GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_mativbo);
        glBufferData(GL_ARRAY_BUFFER, m_meshMatrixIndex.size()*sizeof(unsigned int), &m_meshMatrixIndex[0], GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Render2D::update()
    {
        m_vertices.clear();
        m_matrices.clear();
        m_meshMatrixIndex.clear();
    }

    void Render2D::render()
    {

        m_texture->bind();

        glUniformMatrix4fv(m_modelAttr, m_matrices.size(), GL_FALSE,  glm::value_ptr(m_matrices[0]));

        glEnableVertexAttribArray(m_vertLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glVertexAttribPointer( m_vertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr );

        glEnableVertexAttribArray(m_uvLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glVertexAttribPointer( m_uvLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr );

        glEnableVertexAttribArray(m_matIndAttr);
        glBindBuffer(GL_ARRAY_BUFFER, m_mativbo );
        glVertexAttribIPointer( m_matIndAttr, 1, GL_UNSIGNED_INT, 0, nullptr );

        glDrawArrays(GL_TRIANGLES, 0, m_vertices.size()/2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(m_uvLoc);
        glDisableVertexAttribArray(m_vertLoc);
        glDisableVertexAttribArray(m_matIndAttr);

    }

    Render2D::~Render2D()
    {
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_mativbo);
    }

} // namespace ORCore
