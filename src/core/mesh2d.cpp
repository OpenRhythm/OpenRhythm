#include "mesh2d.hpp"

namespace MgCore
{
	Mesh2D::Mesh2D(ShaderProgram *program)
	: m_program(program)
	{
		init_gl();

	}

	Mesh2D::~Mesh2D()
	{

	}

	void Mesh2D::update()
	{

	}

	void Mesh2D::render()
	{
		glEnableVertexAttribArray(m_vertLoc);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glVertexAttribPointer( m_vertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr );

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisableVertexAttribArray(m_vertLoc);

	}

	void Mesh2D::init_gl()
	{
		m_vertLoc = m_program->vertex_attribute("position");

		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertData), m_vertData, GL_STATIC_DRAW);
	}

	void Mesh2D::scale(float value)
	{

	}

	void Mesh2D::translate(float x, float y)
	{

	}
}