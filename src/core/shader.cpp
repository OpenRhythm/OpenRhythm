#include "vfs.hpp"
#include <glad/glad.h>
#include "shader.hpp"
#include <iostream>

namespace MgCore
{
    Shader::Shader(ShaderInfo _info): info(_info)
    {
        GLint status;
        shader = glCreateShader(info.type);
        std::string data {read_file(info.path)};
        const char *c_str = data.c_str();

        glShaderSource(shader, 1, &c_str, nullptr);
        glCompileShader(shader);

        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

        if (status != GL_TRUE) {
            std::cout << "The shader has failed to compile." << std::endl;
            GLchar message[1024];
            glGetShaderInfoLog(shader, 1024, nullptr, message);
        } else {
            std::cout << "Shader compiled sucessfully." << std::endl;
        }
    }
    Shader::~Shader()
    {
        glDeleteShader(shader);
    }


    ShaderProgram::ShaderProgram(Shader* vertex, Shader* fragment)
    : m_vertex(*vertex), m_fragment(*fragment)
    {
        GLint status;

        m_program = glCreateProgram();

        glAttachShader(m_program, m_vertex.shader);
        glAttachShader(m_program, m_fragment.shader);

        glLinkProgram(m_program);

        glGetProgramiv(m_program, GL_LINK_STATUS, &status);

        if (status != GL_TRUE) {
            std::cout << "The program has failed to link." << std::endl;
            GLchar message[1024];
            glGetProgramInfoLog(m_program, 1024, nullptr, message);
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
        glUseProgram(m_program);
    }

    void ShaderProgram::disuse()
    {
        glUseProgram(0);
    }


    int ShaderProgram::vertex_attribute(std::string name)
    {
        return glGetAttribLocation(m_program, name.c_str());
    }

    int ShaderProgram::uniform_attribute(std::string name)
    {
        return glGetUniformLocation(m_program, name.c_str());
    }


    void ShaderProgram::set_uniform(int uniform, int value)
    {
        glUniform1i(uniform, value);
    }

    void ShaderProgram::set_uniform(int uniform, float value)
    {
        glUniform1f(uniform, value);
    }


    void ShaderProgram::set_uniform(int uniform, const glm::vec2& value)
    {
        glUniform2fv(uniform, 1, &value[0]);
    }

    void ShaderProgram::set_uniform(int uniform, const glm::vec3& value)
    {
        glUniform3fv(uniform, 1, &value[0]);
    }

    void ShaderProgram::set_uniform(int uniform, const glm::vec4& value)
    {
        glUniform4fv(uniform, 1, &value[0]);
    }


    void ShaderProgram::set_uniform(int uniform, const glm::mat2& value)
    {
        glUniformMatrix2fv(uniform, 1, GL_FALSE, &value[0][0]);
    }

    void ShaderProgram::set_uniform(int uniform, const glm::mat3& value)
    {
        glUniformMatrix3fv(uniform, 1, GL_FALSE, &value[0][0]);
    }

    void ShaderProgram::set_uniform(int uniform, const glm::mat4& value)
    {
        glUniformMatrix4fv(uniform, 1, GL_FALSE, &value[0][0]);
    }


    void ShaderProgram::set_uniform(int uniform, const std::array<float, 2>& value)
    {
        glUniform2fv(uniform, 1, &value[0]);
    }

    void ShaderProgram::set_uniform(int uniform, const std::array<float, 3>& value)
    {
        glUniform3fv(uniform, 1, &value[0]);
    }

    void ShaderProgram::set_uniform(int uniform, const std::array<float, 4>& value)
    {
        glUniform4fv(uniform, 1, &value[0]);
    }


    void ShaderProgram::set_uniform(int uniform, const std::array<int, 2>& value)
    {
        glUniform2iv(uniform, 1, &value[0]);
    }

    void ShaderProgram::set_uniform(int uniform, const std::array<int, 3>& value)
    {
        glUniform3iv(uniform, 1, &value[0]);
    }

    void ShaderProgram::set_uniform(int uniform, const std::array<int, 4>& value)
    {
        glUniform4iv(uniform, 1, &value[0]);
    }



}
