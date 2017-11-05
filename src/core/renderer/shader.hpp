// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace ORCore
{
    struct ShaderInfo
    {
        GLuint type;
        std::string path;
    };


    struct Shader
    {
        GLuint shader;
        ShaderInfo info;

        Shader(ShaderInfo);
        Shader(Shader&& other);
        Shader(const Shader& other) = delete; // disable copy

        ~Shader();

        void init_gl();
        void check_error();
    };

    class ShaderProgram
    {
    public:
        // TODO - Implement move and disable copy for Program...
        ShaderProgram();
        ShaderProgram(ShaderProgram&& other);
        ~ShaderProgram();

        ShaderProgram(const ShaderProgram& other) = delete; // disable copy

        void add_shader(Shader&& shader);
        void link();
        
        void check_error();

        void use();
        void disuse();
        operator GLuint();
    private:
        std::vector<Shader> m_shaders;
        GLuint m_program;

    };
} // namespace ORCore
