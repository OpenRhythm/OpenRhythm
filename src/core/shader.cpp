#include <iostream>
#include <memory>
#include <map>
#include "vfs.hpp"
#include "gl.hpp"
#include "shader.hpp"
#include "utils.hpp"

namespace MgCore
{
    static std::map<ShaderType, GLuint> shaderMap
    {
        {ShaderType::Vertex, GL_VERTEX_SHADER_ARB},
        {ShaderType::Fragment, GL_FRAGMENT_SHADER_ARB}
    };

    std::vector<ShaderInfo> shader_parser(std::string path)
    {
        std::vector<ShaderInfo> shaders;

        std::string data {read_file(path)};

        std::vector<std::string> shaderData = splitString(data, "#shader", true);

        for (auto &shaderStr : shaderData) {
            if (shaderStr != "") {
                ShaderInfo shader;
                std::string versionTag = "#version 110\n";
                std::size_t lineEnd = shaderStr.find("\n", 0);
                std::string type = shaderStr.substr(0, lineEnd);
                std::vector<std::string> strs = splitString(type, " ");
                for (auto str : strs) {
                    if (str != "" && str != "#shader") {
                        if (str == "vertex") {
                            shader.type = ShaderType::Vertex;
                        } else if (str == "fragment") {
                            shader.type = ShaderType::Fragment;
                        } else {
                            std::cout << "Unsupported shader type of: " << str << std::endl;
                        }
                    }
                }
                shaderStr = versionTag + shaderStr.substr(lineEnd, shaderStr.size());
                shader.data = shaderStr;
                shader.path = path;
                shaders.push_back(shader);
            }
        }

        return shaders;
    }

    Shader::Shader(ShaderInfo _info): m_info(_info)
    {
        GLint status;
        m_shader = glCreateShaderObjectARB(shaderMap[m_info.type]);
        const char *c_str = m_info.data.c_str();

        glShaderSourceARB(m_shader, 1, &c_str, nullptr);
        glCompileShaderARB(m_shader);

        glGetObjectParameterivARB(m_shader, GL_OBJECT_COMPILE_STATUS_ARB, &status);

        if (status != GL_TRUE) {
            std::cout << "The shader has failed to compile." << std::endl;
            int logLength = 0;

            glGetObjectParameterivARB(m_shader, GL_OBJECT_INFO_LOG_LENGTH_ARB , &logLength);

            auto message = std::unique_ptr<GLchar[]>(new GLchar[logLength]);
            glGetInfoLogARB(m_shader, 1024, nullptr, &message[0]);

            std::cout << message.get() << std::endl;

        } else {
            std::cout << "Shader compiled sucessfully." << std::endl;
        }
    }
    Shader::~Shader()
    {
        glDeleteObjectARB(m_shader);
    }

    unsigned int Shader::get_shader()
    {
        return m_shader;
    }

    ShaderProgram::ShaderProgram(std::string shaderPath)
    {
        GLint status;

        std::vector<ShaderInfo> shaderInfos = shader_parser(shaderPath);

        m_program = glCreateProgramObjectARB();

        for (auto shader : shaderInfos) {
            auto sha = Shader(shader);
            m_shaders.push_back(sha);
            glAttachObjectARB(m_program, sha.get_shader());
        }

        glLinkProgramARB(m_program);

        glGetObjectParameterivARB(m_program, GL_OBJECT_LINK_STATUS_ARB, &status);

        if (status != GL_TRUE) {
            std::cout << "The program has failed to link." << std::endl;
            int logLength = 0;
            glGetObjectParameterivARB(m_program, GL_OBJECT_INFO_LOG_LENGTH_ARB , &logLength);

            auto message = std::unique_ptr<GLchar[]>(new GLchar[logLength]);
            glGetInfoLogARB(m_program, 1024, nullptr, &message[0]);

            std::cout << message.get() << std::endl;

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
