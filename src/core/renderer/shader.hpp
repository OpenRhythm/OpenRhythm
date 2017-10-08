// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#pragma once
#include <string>
#include <array>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace ORCore
{
    struct ShaderInfo
    {
        unsigned int type;
        std::string path;
    };


    struct Shader
    {
        // TODO - Implement move and disable copy for shader...
        GLuint shader;
        ShaderInfo info;
        Shader(ShaderInfo);
        void init_gl();
        ~Shader();
        void check_error();
    };

    class ShaderProgram
    {
    public:
        // TODO - Implement move and disable copy for Program...
        ShaderProgram(Shader& vertex, Shader& fragment);
        ~ShaderProgram();
        
        void check_error();

        void use();
        void disuse();
        operator GLuint();
    private:
        Shader m_vertex;
        Shader m_fragment;
        GLuint m_program;

    };
} // namespace ORCore
