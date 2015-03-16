#include "gl.hpp"
#include "shader.hpp"
#include "glm/glm.hpp"

namespace MgCore
{
	class Mesh2D
	{
	private:
	    ShaderProgram *m_program;
	    GLuint m_vbo;
	    GLuint m_vertLoc;
	    glm::mat4 m_modelMatrix;
	    const GLfloat m_vertData[8] = {
			0.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,
		};

	public:
		Mesh2D(ShaderProgram *program);
		~Mesh2D();
		void render();
		void update();
		void init_gl();
		void scale(float value);
		void translate(float x, float y);
		//void rotate();
	};
}