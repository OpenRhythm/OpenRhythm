#include "config.hpp"
#include "timing.hpp"

#if defined(PLATFORM_LINUX)
    #include <time.h>
#elif defined(PLATFORM_OSX)
    #include <mach/clock.h>
#endif

namespace ORCore
{
    Timer::Timer()
    {

#if defined(PLATFORM_OSX)
        host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &m_cclock);
#elif defined(PLATFORM_WINDOWS)
        QueryPerformanceFrequency(&m_frequency);
#endif
        reset();
    }

    Timer::~Timer()
    {
#if defined(PLATFORM_OSX)
        mach_port_deallocate(mach_task_self(), m_cclock);
#endif
    }

    void Timer::reset()
    {
        m_tickCount = 1;
        m_currentTime = get_time();
        m_previousTime = m_currentTime;
        m_startTime = m_currentTime;
        m_pausedTimeAmount = 0.0;
        m_paused = false;
    }

    double Timer::set_pause(bool pause)
    {
        double time;
        if (pause)
        {
            m_pausedTimeAmount = get_current_time();
        }
        else 
        {
            m_currentTime = get_time();
            m_previousTime = m_currentTime;
            m_startTime = m_currentTime;
        }
        m_paused = pause;
        return m_pausedTimeAmount;

    }

    void Timer::set_resume_target(double negTimeTarget, double revSpeed)
    {
        m_reversalTarget = negTimeTarget;
        m_reversalSpeed = revSpeed;
    }

    bool Timer::is_paused()
    {
        return m_paused;
    }

    double Timer::tick()
    {
        if (m_paused)
        {
            return 0.0;
        }
        else
        {
            m_tickCount++;
            m_previousTime = m_currentTime;
            m_currentTime = get_time();

            return m_currentTime - m_previousTime;
        }
    }

    double Timer::get_current_time() {
        if (m_paused)
        {
            return m_pausedTimeAmount;
        }
        else if (m_reversalTarget != 0.0)
        {
            double time = m_pausedTimeAmount - (m_currentTime - m_startTime) * m_reversalSpeed;

            if (time <= m_reversalTarget)
            {
                m_reversalTarget = 0.0;
                m_reversalSpeed = 1.0;
                m_pausedTimeAmount = time;

                m_currentTime = get_time();
                m_previousTime = m_currentTime;
                m_startTime = m_currentTime;
            }
            return time;
        }
        else
        {
            return m_pausedTimeAmount + (m_currentTime - m_startTime);
        }
    }

// Get time in seconds.
#if defined(PLATFORM_WINDOWS)
    double Timer::get_time()
    {
        LARGE_INTEGER startingTime;
        QueryPerformanceCounter(&startingTime);
        return startingTime.QuadPart / (m_frequency.QuadPart*1.0);
    }

#elif defined(PLATFORM_LINUX)
    double Timer::get_time()
    {
        timespec mt;
        clock_gettime(CLOCK_MONOTONIC, &mt);
        return mt.tv_sec + (mt.tv_nsec * 0.000000001);
    }

#elif defined(PLATFORM_OSX)
    double Timer::get_time()
    {
        mach_timespec_t mts;
        clock_get_time(m_cclock, &mts);
        return mts.tv_sec + (mts.tv_nsec * 0.000000001);
    }

#else
    double Timer::get_time()
    {
        // TODO - implement this fallback with std::chrono::high_resolution_clock
        return 1.0;
    }

#endif

    FpsTimer::FpsTimer()
    :Timer()
    {
        m_fpsPreviousTime = m_currentTime-1; // prevent divide by 0 if get_fps() is called before tick()
    }

    double FpsTimer::get_fps()
    {
        double fps = m_tickCount / (m_currentTime - m_fpsPreviousTime);
        m_tickCount = 0;
        m_fpsPreviousTime = m_currentTime;
        return fps;
    }
} // namespace ORCore
