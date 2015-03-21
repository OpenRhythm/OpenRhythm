#pragma once
#include <string>
#include <memory>
#include "shader.hpp"

namespace MgCore
{

    struct Image
    {
        std::string path;
        int width;
        int height;
        std::unique_ptr<unsigned char[]> pixelData;
        int length;
    };

    Image loadSTB(std::string filename);

    class Texture
    {
    private:
        Image m_image;
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