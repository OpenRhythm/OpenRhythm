#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include "texture.hpp"
#include "vfs.hpp"
#include "png.hpp"

namespace MgCore
{
    static int _texCount = 0;
    static GLuint _currentBoundtexture = 0;

    Image loadPNG(std::string filename)
    {
        std::istringstream file(MgCore::read_file( filename ));
        png::image<png::rgba_pixel> image(file);

        auto pixelBuffer = image.get_pixbuf();

        Image imgData;

        imgData.width = image.get_width();
        imgData.height = image.get_height();
        imgData.length = imgData.width * imgData.height * 4;
        std::unique_ptr<unsigned char[]> data(new unsigned char[imgData.length]());
        imgData.pixelData = std::move(data);

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

    Texture::Texture(std::string path, ShaderProgram *program)
    : m_path(path), m_program(program)
    {
        _texCount++;
        m_texUnitID = _texCount;

        m_image = MgCore::loadPNG(m_path);

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