#include "mesh2d.hpp"
#include <iostream>

namespace MgCore
{
    Mesh2D::Mesh2D(ShaderProgram *program, Texture *texture)
    : m_program(program), m_texture(texture)
    {
        m_vertData[0] = 0.0f;
        m_vertData[1] = 1.0f;
        m_vertData[2] = 1.0f;
        m_vertData[3] = 1.0f;
        m_vertData[4] = 0.0f;
        m_vertData[5] = 0.0f;
        m_vertData[6] = 1.0f;
        m_vertData[7] = 0.0f;
        init_gl();
        m_xScale = 0.0f;
        m_yScale = 0.0f;
        m_xPos = 0.0f;
        m_yPos = 0.0f;
    }

    Mesh2D::~Mesh2D()
    {

        glDeleteBuffers(1, &m_vbo);

    }

    void Mesh2D::update()
    {
        // yes this is going to be inefficient... ill fix it later.
        m_modelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), m_vecTrans), m_vecScale);
    }

    void Mesh2D::render()
    {
        m_program->set_uniform(m_modelAttr, m_modelMatrix);
        m_texture->bind();
        glEnableVertexAttribArray(m_vertLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glVertexAttribPointer( m_vertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr );

        glEnableVertexAttribArray(m_uvLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_uvbo);
        glVertexAttribPointer( m_uvLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr );

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(m_uvLoc);
        glDisableVertexAttribArray(m_vertLoc);

    }

    void Mesh2D::init_gl()
    {
        m_vertLoc = m_program->vertex_attribute("position");
        m_uvLoc = m_program->vertex_attribute("vertexUV");
        m_modelAttr = m_program->uniform_attribute("model");

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertData), m_vertData, GL_STATIC_DRAW);

        glGenBuffers(1, &m_uvbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_uvbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertData), m_vertData, GL_STATIC_DRAW);
    }

    void Mesh2D::scale(float x, float y)
    {
        m_xScale = x;
        m_yScale = y;
        m_vecScale = glm::vec3(m_xScale, m_yScale, 0.0f);
    }

    void Mesh2D::translate(float x, float y)
    {
        m_xPos = x;
        m_yPos = y;
        m_vecTrans = glm::vec3(m_xPos, m_yPos, 0.0f);
    }
}