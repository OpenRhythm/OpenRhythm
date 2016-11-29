#include "config.hpp"
#include "timing.hpp"

#if defined(PLATFORM_LINUX)
    #include <time.h>
#elif defined(PLATFORM_OSX)
    #include <mach/clock.h>
#endif

namespace ORCore
{

// Get time in milliseconds.
#if defined(PLATFORM_WINDOWS)
    double Timer::get_time()
    {
        LARGE_INTEGER startingTime;
        QueryPerformanceCounter(&startingTime);
        return (startingTime.QuadPart * 1000.0) / m_frequency.QuadPart;
    }

#elif defined(PLATFORM_LINUX)
    double Timer::get_time()
    {
        timespec mt;
        clock_gettime(CLOCK_MONOTONIC, &mt);
        return (mt.tv_sec * 1000.0) + (mt.tv_nsec * 0.000001);
    }

#elif defined(PLATFORM_OSX)
    double Timer::get_time()
    {
        mach_timespec_t mts;
        clock_get_time(m_cclock, &mts);
        return (mts.tv_sec * 1000.0) + (mts.tv_nsec * 0.000001);
    }

#else
    double Timer::get_time()
    {
        return 1000.0;
    }

#endif


    Timer::Timer()
    {

#if defined(PLATFORM_OSX)
        host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &m_cclock);
#elif defined(PLATFORM_WINDOWS)
        QueryPerformanceFrequency(&m_frequency);
#endif
        m_tickCount = 1;
        m_currentTime = get_time();
        m_previousTime = m_currentTime;
        m_startTime = m_currentTime;
    }

    Timer::~Timer()
    {
#if defined(PLATFORM_OSX)
        mach_port_deallocate(mach_task_self(), m_cclock);
#endif
    }

    double Timer::tick()
    {
        m_tickCount++;
        m_previousTime = m_currentTime;
        m_currentTime = get_time();

        return m_currentTime - m_previousTime;
    }

    double Timer::get_current_time() {
        return m_currentTime - m_startTime;
    }

    FpsTimer::FpsTimer()
    :Timer()
    {
        m_fpsPreviousTime = m_currentTime-1; // prevent divide by 0 if get_fps() is called before tick()
    }

    double FpsTimer::get_fps()
    {
        double fps = (m_tickCount*1000.0) / (m_currentTime - m_fpsPreviousTime);
        m_tickCount = 0;
        m_fpsPreviousTime = m_currentTime;
        return fps;
    }
} // namespace ORCore
