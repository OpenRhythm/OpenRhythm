// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "config.hpp"
#include "texture.hpp"
#include <iostream>

#include "filesystem.hpp"

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
    // TODO - This doesn't really fit here anymore, should find a better place for it.
    //        Could call it asset loaders or something smf could be moved there as well.
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
        {
            std::cout << _("Failed to get image data") << std::endl;
        }


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


    std::vector<GLenum> TextureBase::sm_freedBindingPoints;
    GLuint TextureBase::sm_bindpointTail = 0;
    int TextureBase::sm_textureCount = 0;

    TextureBase::TextureBase(GLenum targetType)
    :m_texTargetType(targetType)
    {
        m_texBindingPoint = 0;
        sm_textureCount++;
        m_texID = sm_textureCount;
        m_texIsBound = false;
    }

    TextureBase::~TextureBase()
    {
        unbind();
    }

    void TextureBase::bind(GLuint location)
    {
        if (m_texIsBound == false)
        {
            m_texBindingPoint = aquire_bindpoint();
            m_texIsBound = true;

            glActiveTexture(GL_TEXTURE0+m_texBindingPoint);
            glBindTexture(m_texTargetType, m_oglTexID);
            // std::cout << _("Texture bound") << std::endl;
        }

        glUniform1i(location, m_texBindingPoint);
    }

    void TextureBase::unbind()
    {
        if (m_texIsBound == true)
        {

            glActiveTexture(GL_TEXTURE0+m_texBindingPoint);
            glBindTexture(m_texTargetType, 0);
            release_bindpoint(m_texBindingPoint);
            m_texBindingPoint = 0;
            m_texIsBound = false;
            glActiveTexture(GL_TEXTURE0);
            // std::cout << _("Texture unbound") << std::endl;
        }
    }

    int TextureBase::get_id() {
        return m_texID;
    }

    GLenum TextureBase::aquire_bindpoint()
    {
        GLenum rtnPoint;
        if (sm_freedBindingPoints.size() > 0)
        {
            rtnPoint = sm_freedBindingPoints.back();
            sm_freedBindingPoints.pop_back();
        } else {
            // TODO - sm_bindpointTail shouldn't be longer than GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS.
            sm_bindpointTail++;
            rtnPoint = sm_bindpointTail;

        }
        return rtnPoint;
    }

    void TextureBase::release_bindpoint(GLenum bindpoint)
    {
        sm_freedBindingPoints.push_back(bindpoint);
    }

    Texture::Texture(GLenum targetType)
    :TextureBase(targetType)
    {
        init_gl();
    }


    void Texture::init_gl()
    {
        glGenTextures(1, &m_oglTexID);
        glBindTexture(m_texTargetType, m_oglTexID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(m_texTargetType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(m_texTargetType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glBindTexture(m_texTargetType, 0);
    }

    void Texture::update_image_data(Image& img)
    {
        glBindTexture(m_texTargetType, m_oglTexID);
        glTexImage2D(m_texTargetType, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(img.pixelData.get())[0]);
        glGenerateMipmap(m_texTargetType);
    }

    BufferTexture::BufferTexture(GLenum bufferType)
    :m_bufferType(bufferType), TextureBase(GL_TEXTURE_BUFFER)
    {
        init_gl();
    }

    void BufferTexture::init_gl()
    {
        glGenTextures(1, &m_oglTexID);
    }

    void BufferTexture::assign_buffer(GLuint buffer)
    {
        glBindTexture(m_texTargetType, m_oglTexID);
        glTexBuffer(m_texTargetType, m_bufferType, buffer);
    }

} // namespace ORCore
