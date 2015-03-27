#include "gl.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include "context.hpp"

namespace MgCore
{

    Context::Context(int major, int minor)
    : m_major(major), m_minor(minor)
    {
        m_profile = SDL_GL_CONTEXT_PROFILE_CORE;

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, m_major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, m_minor);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, m_profile);
    }

    Context::Context()
    {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    }

    Context::~Context()
    {
        SDL_GL_DeleteContext(m_context);
    }

    bool Context::check_hardware()
    {
        bool systemSupported = true;
        MgCore::GraphicsInfo conInfo = get_info();

        std::vector<std::string> conExt = conInfo.extensions;

        if (conInfo.versionMajor == 1 && conInfo.versionMinor < 4 ) {
            systemSupported = false;
        } else if (conInfo.versionMajor < 2) {
            auto comp = [] (const std::string &value) {

                if (value != "ARB_vertex_array_object") {
                    return false;
                } else if (value != "GL_ARB_fragment_shader") {
                    return false;
                } else if (value != "GL_ARB_vertex_shader") {
                    return false;
                } else if (value != "GL_ARB_shader_objects") {
                    return false;
                } else if (value != "GL_ARB_vertex_buffer_object") {
                    return false;
                }

                return true;
            };

            if (std::find_if(conExt.begin(), conExt.end(), comp) != conExt.end()) {
                systemSupported = false;

            }

        }

        return systemSupported;
    }

    GraphicsInfo Context::get_info()
    {
        GraphicsInfo info;
        info.version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
        glGetIntegerv(GL_MAJOR_VERSION, &info.versionMajor);
        glGetIntegerv(GL_MINOR_VERSION, &info.versionMinor);
        info.glsl = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
        info.renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
        info.vendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
        int numExt = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);
        std::string ext = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));
        std::stringstream extStream(ext);
        std::string item;
        for (int i = 0; i < numExt; i++) {
            std::getline(extStream, item, ' ');
            info.extensions.push_back(item);
        }

        return info;
    }

    void Context::set_window(Window* window)
    {
        m_window = window;
        if (m_context == nullptr) {
            m_context = SDL_GL_CreateContext(m_window->get_platform_window());
        }
    }
}
