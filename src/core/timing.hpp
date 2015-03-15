#include "config.hpp"

#if defined(PLATFORM_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#elif defined(PLATFORM_LINUX)
    #include <time.h>
#elif defined(PLATFORM_OSX)
    #include <mach/clock.h>
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
        double m_fpsTime;
        double m_delta;
        float m_fps;

#if defined(PLATFORM_WINDOWS)
        double get_time()
        {
            return 1.0;
        }
#elif defined(PLATFORM_LINUX)
        double get_time()
        {
            timespec mt;
            clock_gettime(CLOCK_MONOTONIC, &mt);
            return mt.tv_sec + (mt.tv_nsec * 0.000000001);
        }
#elif defined(PLATFORM_OSX)
        clock_serv_t m_cclock;
        double get_time()
        {
            mach_timespec_t mts;
            clock_get_time(m_cclock, &mts);
            return mts.tv_sec + (mts.tv_nsec * 0.000000001);
        }
#else
        double get_time()
        {
            return 1.0;
        }

#endif

    public:
        FpsTimer();
        ~FpsTimer();
        double tick();
        float get_fps();
    };
}