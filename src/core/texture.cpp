#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include "texture.hpp"
#include "vfs.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include "stb_image.h"

namespace MgCore
{
    static int _texCount = 0;
    static GLuint _currentBoundtexture = 0;

    Image loadImage(std::string filename)
    {
        std::string mem_buf = MgCore::read_file( filename );
        unsigned char *img_buf;
        Image imgData;
        int comp;

        img_buf = stbi_load_from_memory( (unsigned char*)mem_buf.c_str(), mem_buf.size(), &imgData.width, &imgData.height, &comp, 0 );
        imgData.length = imgData.width * imgData.height * 4;
        std::unique_ptr<unsigned char[]> data(new unsigned char[imgData.length]());
        imgData.pixelData = std::move(data);

        for (int i; i < imgData.length; i++)
            imgData.pixelData[i] = img_buf[i];

        return imgData;
    }

    Texture::Texture(std::string path, ShaderProgram *program)
    : m_path(path), m_program(program)
    {
        _texCount++;
        m_texUnitID = _texCount;

        m_image = MgCore::loadImage(m_path);

        m_texSampID = m_program->uniform_attribute("textureSampler");

        GLuint texid;
        glGenTextures(1, &texid);
        m_texID = texid;
        glBindTexture(GL_TEXTURE_2D, m_texID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.width, m_image.height,
                0, GL_RGBA, GL_UNSIGNED_BYTE, &(m_image.pixelData.get())[0]);

    }

    void Texture::bind()
    {
        if (_currentBoundtexture != m_texUnitID) {
            _currentBoundtexture = m_texUnitID;

            glActiveTexture(GL_TEXTURE0+m_texUnitID);
            glBindTexture(GL_TEXTURE_2D, m_texID);
            m_program->set_uniform(m_texSampID, m_texUnitID);
            std::cout << "Texture bound" << std::endl;

        }
    }
}