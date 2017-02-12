#pragma once
#include <string>
#include <memory>
#include <vector>
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

    class TextureBase
    {
    public:
        TextureBase(GLenum targetType);
        virtual ~TextureBase();
        void bind(GLuint location);
        void unbind();
        int get_id(); // Internal texture ID
    protected:
        GLenum aquire_bindpoint();
        void release_bindpoint(GLenum bindpoint);

        static std::vector<GLenum> sm_freedBindingPoints;
        static GLuint sm_bindpointTail;
        static int sm_textureCount;

        int m_texID;
        GLenum m_texBindingPoint;
        bool m_texIsBound;
        GLuint m_oglTexID;
        GLenum m_texTargetType;
    };

    class Texture : public TextureBase
    {
    public:
        Texture(GLenum targetType);
        void init_gl();
        void update_image_data(Image& img);
    private:

        GLenum m_texFormat;
    };

    class BufferTexture : public TextureBase
    {
    public:
        BufferTexture(GLenum bufferType);
        void init_gl();
        void assign_buffer(GLuint buffer);
    private:
        GLenum m_bufferType;
    };

} // namespace ORCore
