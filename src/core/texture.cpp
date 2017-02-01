#include "config.hpp"
#include "texture.hpp"
#include <iostream>

#include "vfs.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
//exclude image formats
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STBI_NO_PSD
#define STBI_NO_GIF
// This leaves us with the following supported image formats:
// jpg  png  bmp  tga  hdr
#ifdef _MSC_VER
#   pragma warning(push, 0)
#   endif
#include <stb_image.h>
#   ifdef _MSC_VER
#   pragma warning(pop)
#endif

namespace ORCore
{
    static int _texCount = 0;
    static GLuint _currentBoundtexture = 0;

    Image loadSTB(std::string filename)
    {
        std::string mem_buf = ORCore::read_file( filename, FileMode::Binary );

        Image imgData;

        auto conv_mem = std::make_unique<unsigned char[]>( mem_buf.size() );

        // to prevent potential issues convert each value seperately
        // one could cast the int* to unsigned int* however this could have large issues there can be
        // platform differances on how this is imeplemented.
        for (size_t i = 0; i < mem_buf.size(); i++) {
            conv_mem[i] = static_cast<unsigned char>(mem_buf[i]);
        }

        unsigned char *img_buf;
        int comp;

        img_buf = stbi_load_from_memory( &conv_mem[0], mem_buf.size(), &imgData.width, &imgData.height, &comp, 0 );

        if ( img_buf == nullptr )
            std::cout << _("Failed to get image data") << std::endl;


        imgData.length = imgData.width * imgData.height * 4;
		imgData.pixelData = std::make_unique<unsigned char[]>(imgData.length);

        // We have to copy the data out of the returned data from stb.
        // This was an issue with the previous implementation where the data was destroyed before
        // opengl could copy the data to the gpu. OpenGL may even require the image data to be persistant
        // cpu side if not it may be a good idea to do so anyways.
        int i, j;
        // currently the main reason the loop is setup like this, is that it makes it a bit easier to convert RGB to RGBA.
        for (int x = 0;x < imgData.width; x++) {
            for (int y = 0; y < imgData.height; y++) {
                i = 4 * (y * imgData.width + x);
                j = comp * (y * imgData.width + x);
                imgData.pixelData[i+0] = img_buf[j+0];
                imgData.pixelData[i+1] = img_buf[j+1];
                imgData.pixelData[i+2] = img_buf[j+2];
                if (comp == 4) {
                    imgData.pixelData[i+3] = img_buf[j+3];
                } else {
                    imgData.pixelData[i+3] = 255U;
                }
            }
        }
        stbi_image_free( img_buf );
        return std::move(imgData);
    }


    Texture::Texture(std::string path, ShaderProgram *program)
    : m_program(program), m_path(path)
    {
        _texCount++;
        m_texUnitID = _texCount;

        m_image = ORCore::loadSTB(m_path);

        m_texSampID = m_program->uniform_attribute("textureSampler");

        GLuint texid;
        glGenTextures(1, &texid);
        m_texID = texid;
        glBindTexture(GL_TEXTURE_2D, m_texID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.width, m_image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(m_image.pixelData.get())[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void Texture::bind()
    {
        if (_currentBoundtexture != (GLuint)m_texUnitID) {
            _currentBoundtexture = m_texUnitID;

            glActiveTexture(GL_TEXTURE0+m_texUnitID);
            glBindTexture(GL_TEXTURE_2D, m_texID);
            m_program->set_uniform(m_texSampID, m_texUnitID);
            std::cout << _("Texture bound") << std::endl;

        }
    }

    int Texture::get_id() {
        return m_texUnitID;
    }
    
} // namespace ORCore
