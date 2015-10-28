#pragma once
#include "config.hpp"

#if defined(PLATFORM_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#elif defined(PLATFORM_OSX)
    #include <mach/mach.h>
#endif

namespace MgCore
{
    class FpsTimer
    {
    private:
        int m_frames;
        double m_currentTime;
        double m_previousTime;
        double m_delta;
        double m_fpsTime;
        double m_fps;

#if defined(PLATFORM_WINDOWS)
        LARGE_INTEGER m_frequency;
#elif defined(PLATFORM_OSX)
        clock_serv_t m_cclock;
#endif

        double get_time();

    public:
        FpsTimer();
        ~FpsTimer();
        double tick();
        double get_fps();
    };
}
