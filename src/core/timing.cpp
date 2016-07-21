#include "timing.hpp"

#if defined(PLATFORM_LINUX)
    #include <time.h>
#elif defined(PLATFORM_OSX)
    #include <mach/clock.h>
#endif

namespace MgCore
{

#if defined(PLATFORM_WINDOWS)
    double FpsTimer::get_time()
    {
        LARGE_INTEGER startingTime;
        QueryPerformanceCounter(&startingTime);
        return startingTime.QuadPart / static_cast<double>(m_frequency.QuadPart);
    }

#elif defined(PLATFORM_LINUX)
    double FpsTimer::get_time()
    {
        timespec mt;
        clock_gettime(CLOCK_MONOTONIC, &mt);
        return mt.tv_sec + (mt.tv_nsec * 0.000000001);
    }

#elif defined(PLATFORM_OSX)
    double FpsTimer::get_time()
    {
        mach_timespec_t mts;
        clock_get_time(m_cclock, &mts);
        return mts.tv_sec + (mts.tv_nsec * 0.000000001);
    }

#else
    double FpsTimer::get_time()
    {
        return 1.0;
    }

#endif


    FpsTimer::FpsTimer()
    {

#if defined(PLATFORM_OSX)
        host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &m_cclock);
#elif defined(PLATFORM_WINDOWS)
        QueryPerformanceFrequency(&m_frequency);
#endif
        m_frames = 1;
        m_currentTime = get_time();
        m_previousTime = m_currentTime;
        m_startTime = m_currentTime;
        m_delta = 0.0;
        m_fpsTime = 0.0;
        m_fps = 0.0f;
    }

    FpsTimer::~FpsTimer()
    {
#if defined(PLATFORM_OSX)
        mach_port_deallocate(mach_task_self(), m_cclock);
#endif
    }

    double FpsTimer::tick()
    {
        m_frames++;
        m_previousTime = m_currentTime;
        m_currentTime = get_time();

        m_delta = m_currentTime - m_previousTime;
        m_fpsTime += m_delta;

        return m_delta;
    }

    double FpsTimer::get_current_time() {
        return m_currentTime - m_startTime;
    }

    double FpsTimer::get_fps()
    {
        if (m_fpsTime == 0) {
            m_fpsTime += 1;
        }

        m_fps = m_frames / m_fpsTime;
        m_fpsTime = 0;
        m_frames = 0;
        return m_fps;
    }
}
