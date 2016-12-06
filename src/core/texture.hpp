#pragma once
#include <string>
#include <memory>
#include <glad/glad.h>

#include "shader.hpp"

namespace ORCore
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
    public:
        Texture(std::string path, ShaderProgram *program);
        void bind();
        int get_id();
        //~Texture();

    private:
        Image m_image;
        ShaderProgram *m_program;
        std::string m_path;
        int m_texSampID;
        int m_texUnitID;
        GLuint m_texID;

    };
} // namespace ORCore
