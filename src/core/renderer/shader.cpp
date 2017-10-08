// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "config.hpp"
#include <iostream>
#include <stdexcept>

#include <spdlog/spdlog.h>
#include "filesystem.hpp"
#include "shader.hpp"

namespace ORCore
{
    static std::shared_ptr<spdlog::logger> logger;

    Shader::Shader(ShaderInfo _info): info(_info)
    {

        logger = spdlog::get("default");
        init_gl();
    }

    void Shader::init_gl()
    {
        shader = glCreateShader(info.type);
        std::string data {read_file(info.path)};
        const char *c_str = data.c_str();

        glShaderSource(shader, 1, &c_str, nullptr);
        glCompileShader(shader);
    }

    Shader::~Shader()
    {
        glDeleteShader(shader);
    }

    void Shader::check_error()
    {
        GLint status;

        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE) {
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

            auto logData = std::make_unique<GLchar[]>(length+1);
            logData[length] = '\0'; // Make sure it is null terminated.

            glGetShaderInfoLog(shader, length, nullptr, logData.get());

            logger->error(logData.get());

            throw std::runtime_error(_("Shader compilation failed."));

        } else {
            logger->info(_("Shader compiled sucessfully."));
        }
    }


    ShaderProgram::ShaderProgram(Shader& vertex, Shader& fragment)
    : m_vertex(vertex), m_fragment(fragment)
    {
        logger = spdlog::get("default");
        m_program = glCreateProgram();

        glAttachShader(m_program, m_vertex.shader);
        glAttachShader(m_program, m_fragment.shader);

        glLinkProgram(m_program);
    }

    ShaderProgram::~ShaderProgram()
    {
        glDeleteProgram(m_program);
    }

    void ShaderProgram::check_error()
    {
        // We want to check the compile/link status of the shaders all at once.
        // At some place that isn't right after the shader compilation step.
        // That way the drivers can better parallelize shader compilation.
        m_vertex.check_error();
        m_fragment.check_error();

        GLint status;

        glGetProgramiv(m_program, GL_LINK_STATUS, &status);
        if (status != GL_TRUE) {
            GLint length;
            glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);

            auto logData = std::make_unique<GLchar[]>(length+1);
            logData[length] = '\0'; // Make sure it is null terminated.

            glGetProgramInfoLog(m_program, length, nullptr, logData.get());
            logger->error(logData.get());

            throw std::runtime_error(_("Shader linkage failed."));
        } else {
            logger->info(_("Shader linked sucessfully."));
        }
    }


    void ShaderProgram::use()
    {
        glUseProgram(m_program);
    }

    void ShaderProgram::disuse()
    {
        glUseProgram(0);
    }


    ShaderProgram::operator GLuint()
    {
        return m_program;
    }


} // namespace ORCore
