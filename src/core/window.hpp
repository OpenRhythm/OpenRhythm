#pragma once
#include <string>

#include <SDL.h>
#undef main
#include "context.hpp"

namespace MgCore
{
    class Context;
    const Uint32 defaultWindowFlag = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    void init_video();

    class Window
    {
    private:
        std::string m_title;
        bool m_fullscreen;

        int m_width;
        int m_height;
        int m_x;
        int m_y;

        Uint32 m_sdlFlags;
        SDL_Window* m_sdlWindow;

        Context* m_context;

    public:

        Window(int width=800,
                int height=600,
                bool fullscreen=false,
                std::string title="Game");

        ~Window();

        SDL_Window* get_platform_window() {return m_sdlWindow;}

        void make_current(Context* context);
        void flip();

    };

}
