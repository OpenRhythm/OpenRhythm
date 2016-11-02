#include "context.hpp"

namespace ORCore
{
    Context::Context(int major, int minor, int msaa)
    : m_major(major), m_minor(minor), m_msaa(msaa)
    {
        m_profile = SDL_GL_CONTEXT_PROFILE_CORE;

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, m_major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, m_minor);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, m_profile);

        if (m_msaa < 0) {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, m_msaa);
        }
    }

    Context::~Context()
    {
        SDL_GL_DeleteContext(m_context);
    }

    void Context::set_window(Window* window)
    {
        m_window = window;
        if (m_context == nullptr) {
            m_context = SDL_GL_CreateContext(m_window->get_platform_window());
        }
    }

    SDL_GLContext Context::get_platform_context() {
        return m_context;
    }
} // namespace ORCore
