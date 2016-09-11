#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include <glad/glad.h>
#include "vfs.hpp"
#include "texture.hpp"

#if USE_LIB_PNGCPP
#include <png.hpp>
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
//exclude image formats
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STBI_NO_PSD
#define STBI_NO_GIF
// This leaves us with the following supported image formats:
// jpg  png  bmp  tga  hdr
#include <stb_image.h>

namespace ORCore
{
    static int _texCount = 0;
    static GLuint _currentBoundtexture = 0;


#if USE_LIB_PNGCPP
    Image loadPNG(std::string filename)
    {
        std::istringstream file(ORCore::read_file( filename, FileMode::Binary ));
        png::image<png::rgba_pixel> image(file);

        auto pixelBuffer = image.get_pixbuf();

        Image imgData;

        imgData.width = image.get_width();
        imgData.height = image.get_height();
        imgData.length = imgData.width * imgData.height * 4;
        imgData.pixelData = std::make_unique<unsigned char[]>(imgData.length);

        int i = 0;

        for (int x = 0;x < imgData.width; x++) {
            for (int y = 0; y < imgData.height; y++) {
                auto pixel = pixelBuffer.get_pixel(x, y);
                i = 4 * (y * imgData.width + x);

                imgData.pixelData[i+0] = pixel.red;
                imgData.pixelData[i+1] = pixel.green;
                imgData.pixelData[i+2] = pixel.blue;
                imgData.pixelData[i+3] = pixel.alpha;

            }
        }
        return imgData;
    }
#endif

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
            std::cout << "Failed to get image data" << std::endl;


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
    : m_path(path), m_program(program)
    {
        _texCount++;
        m_texUnitID = _texCount;

#if USE_LIB_PNGCPP
        m_image = ORCore::loadPNG(m_path);
#else
        m_image = ORCore::loadSTB(m_path);
#endif

        m_texSampID = m_program->uniform_attribute("textureSampler");

        GLuint texid;
        glGenTextures(1, &texid);
        m_texID = texid;
        glBindTexture(GL_TEXTURE_2D, m_texID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.width, m_image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(m_image.pixelData.get())[0]);
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
