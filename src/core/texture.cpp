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

    Texture::Texture(std::string path, ShaderProgram *program)
    : m_path(path), m_program(program)
    {
        std::string mem_buf = MgCore::read_file( m_path );
        unsigned char *img_buf;
        int width, height, comp;
        int format;

        img_buf = stbi_load_from_memory( (unsigned char*)mem_buf.c_str(), mem_buf.size(), &width, &height, &comp, 0 );

        if ( img_buf == NULL )
            std::cout << "Failed to get image data" << std::endl;

        switch ( comp )
        {
            case 3: format = GL_RGB;
            default:
            case 4: format = GL_RGBA;
        }

        _texCount++;
        m_texUnitID = _texCount;

        m_texSampID = m_program->uniform_attribute("textureSampler");

        GLuint texid;
        glGenTextures(1, &texid);
        m_texID = texid;
        glBindTexture(GL_TEXTURE_2D, m_texID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, &img_buf);

        stbi_image_free( img_buf );
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