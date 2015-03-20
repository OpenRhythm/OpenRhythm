#include "vfs.hpp"
#include "gl.hpp"
#include "shader.hpp"
#include <iostream>

namespace MgCore
{
    Shader::Shader(ShaderInfo _info): info(_info)
    {
        GLint status;
        shader = glCreateShaderObjectARB(info.type);
        std::string data {read_file(info.path)};
        const char *c_str = data.c_str();

        glShaderSourceARB(shader, 1, &c_str, nullptr);
        glCompileShaderARB(shader);

        glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &status);

        if (status != GL_TRUE) {
            std::cout << "The shader has failed to compile." << std::endl;
            GLchar message[1024];
            glGetInfoLogARB(shader, 1024, nullptr, message);
        } else {
            std::cout << "Shader compiled sucessfully." << std::endl;
        }
    }
    Shader::~Shader()
    {
        glDeleteObjectARB(shader);
    }


    ShaderProgram::ShaderProgram(Shader* vertex, Shader* fragment)
    : m_vertex(*vertex), m_fragment(*fragment)
    {
        GLint status;

        m_program = glCreateProgramObjectARB();

        glAttachObjectARB(m_program, m_vertex.shader);
        glAttachObjectARB(m_program, m_fragment.shader);

        glLinkProgramARB(m_program);

        glGetObjectParameterivARB(m_program, GL_OBJECT_LINK_STATUS_ARB, &status);

        if (status != GL_TRUE) {
            std::cout << "The program has failed to link." << std::endl;
            GLchar message[1024];
            glGetInfoLogARB(m_program, 1024, nullptr, message);
            std::cout << message << std::endl;
        } else {
            std::cout << "Program linked sucessfully." << std::endl;
        }
    }

    ShaderProgram::~ShaderProgram()
    {
    }


    void ShaderProgram::use()
    {
        glUseProgramObjectARB(m_program);
    }

    void ShaderProgram::disuse()
    {
        glUseProgramObjectARB(0);
    }


    int ShaderProgram::vertex_attribute(std::string name)
    {
        return glGetAttribLocationARB(m_program, name.c_str());
    }

    int ShaderProgram::uniform_attribute(std::string name)
    {
        return glGetUniformLocationARB(m_program, name.c_str());
    }


    void ShaderProgram::set_uniform(int uniform, int value)
    {
        glUniform1iARB(uniform, value);
    }

    void ShaderProgram::set_uniform(int uniform, float value)
    {
        glUniform1fARB(uniform, value);
    }


    void ShaderProgram::set_uniform(int uniform, const glm::vec2& value)
    {
        glUniform2fvARB(uniform, 1, &value[0]);
    }

    void ShaderProgram::set_uniform(int uniform, const glm::vec3& value)
    {
        glUniform3fvARB(uniform, 1, &value[0]);
    }

    void ShaderProgram::set_uniform(int uniform, const glm::vec4& value)
    {
        glUniform4fvARB(uniform, 1, &value[0]);
    }


    void ShaderProgram::set_uniform(int uniform, const glm::mat2& value)
    {
        glUniformMatrix2fvARB(uniform, 1, GL_FALSE, &value[0][0]);
    }

    void ShaderProgram::set_uniform(int uniform, const glm::mat3& value)
    {
        glUniformMatrix3fvARB(uniform, 1, GL_FALSE, &value[0][0]);
    }

    void ShaderProgram::set_uniform(int uniform, const glm::mat4& value)
    {
        glUniformMatrix4fvARB(uniform, 1, GL_FALSE, &value[0][0]);
    }


    void ShaderProgram::set_uniform(int uniform, const std::array<float, 2>& value)
    {
        glUniform2fvARB(uniform, 1, &value[0]);
    }

    void ShaderProgram::set_uniform(int uniform, const std::array<float, 3>& value)
    {
        glUniform3fvARB(uniform, 1, &value[0]);
    }

    void ShaderProgram::set_uniform(int uniform, const std::array<float, 4>& value)
    {
        glUniform4fvARB(uniform, 1, &value[0]);
    }


    void ShaderProgram::set_uniform(int uniform, const std::array<int, 2>& value)
    {
        glUniform2ivARB(uniform, 1, &value[0]);
    }

    void ShaderProgram::set_uniform(int uniform, const std::array<int, 3>& value)
    {
        glUniform3ivARB(uniform, 1, &value[0]);
    }

    void ShaderProgram::set_uniform(int uniform, const std::array<int, 4>& value)
    {
        glUniform4ivARB(uniform, 1, &value[0]);
    }



}
