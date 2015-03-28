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

        glDeleteBuffersARB(1, &m_vbo);

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
        glEnableVertexAttribArrayARB(m_vertLoc);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbo);
        glVertexAttribPointerARB( m_vertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr );

        glEnableVertexAttribArrayARB(m_uvLoc);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_uvbo);
        glVertexAttribPointerARB( m_uvLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr );

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glDisableVertexAttribArrayARB(m_uvLoc);
        glDisableVertexAttribArrayARB(m_vertLoc);

    }

    void Mesh2D::init_gl()
    {
        m_vertLoc = m_program->vertex_attribute("position");
        m_uvLoc = m_program->vertex_attribute("vertexUV");
        m_modelAttr = m_program->uniform_attribute("model");

        glGenBuffersARB(1, &m_vbo);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbo);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(m_vertData), m_vertData, GL_STATIC_DRAW_ARB);

        glGenBuffersARB(1, &m_uvbo);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_uvbo);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(m_vertData), m_vertData, GL_STATIC_DRAW_ARB);
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