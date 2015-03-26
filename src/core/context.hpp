#pragma once
#include "SDL.h"
#undef main

#include <string>
#include "window.hpp"

namespace MgCore
{

    struct GraphicsInfo
    {
        std::string version;
        int versionMajor;
        int versionMinor;
        std::string glsl;
        std::string renderer;
        std::string vendor;
    };

    class Window;
    
    class Context
    {
    private:
        int m_major;
        int m_minor;
        int m_msaa;
        int m_profile;

        Window* m_window = nullptr;
        SDL_GLContext m_context = nullptr;


    public:

        Context(int major, int minor, int msaa);
        ~Context();
        GraphicsInfo get_info();
        void set_window(Window* window);
        SDL_GLContext get_platform_context() {return m_context;}
    };
}