#include "gl.hpp"
#include "shader.hpp"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

namespace MgCore
{
	class Mesh2D
	{
	private:
	    ShaderProgram *m_program;
	    GLuint m_vbo;
	    GLuint m_vertLoc;
	    GLuint m_modelAttr;
	    glm::mat4 m_modelMatrix;
		glm::vec3 m_vecTrans;
		glm::vec3 m_vecScale;
	    float m_xScale;
	    float m_yScale;
	    float m_xPos;
	    float m_yPos;
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
		void scale(float x, float y);
		void translate(float x, float y);
		//void rotate();
	};
}