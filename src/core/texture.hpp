#pragma once
#include <string>
#include <memory>
#include "gl.hpp"
#include "shader.hpp"

namespace MgCore
{
    class Texture
    {
    private:
        ShaderProgram *m_program;
        std::string m_path;
        int m_texSampID;
        int m_texUnitID;
        GLuint m_texID;

    public:
        Texture(std::string path, ShaderProgram *program);
        void bind();
        //~Texture();
    };
}