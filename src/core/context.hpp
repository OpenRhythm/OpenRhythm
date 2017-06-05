// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#pragma once
#include <SDL.h>
#undef main

#include "window.hpp"

namespace ORCore
{
    class Window;

    class Context
    {
    public:

        Context(int major, int minor, int msaa);
        ~Context();
        void set_window(Window* window);
        SDL_GLContext get_platform_context();

    private:
        int m_major;
        int m_minor;
        int m_msaa;
        int m_profile;

        Window* m_window = nullptr;
        SDL_GLContext m_context = nullptr;
    };

} // namespace ORCore
