#pragma once
#include "config.hpp"

#if defined(PLATFORM_WINDOWS)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#elif defined(PLATFORM_OSX)
    #include <mach/mach.h>
#endif

namespace ORCore
{
    class FpsTimer
    {
    public:
        FpsTimer();
        ~FpsTimer();
        double tick();
        double get_current_time();
        double get_fps();

    private:
        int m_frames;
        double m_currentTime;
        double m_previousTime;
        double m_startTime;
        double m_delta;
        double m_fpsTime;
        double m_fps;

#if defined(PLATFORM_WINDOWS)
        LARGE_INTEGER m_frequency;
#elif defined(PLATFORM_OSX)
        clock_serv_t m_cclock;
#endif

        double get_time();


    };
}
