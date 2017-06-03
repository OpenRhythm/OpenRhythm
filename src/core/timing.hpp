#pragma once

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
    class Timer
    {
    public:
        Timer();
        virtual ~Timer();
        void reset();
        double set_pause(bool pause);
        void set_resume_target(double negTimeTarget, double revSpeed);
        bool is_paused();
        double tick();
        double get_current_time();

    protected:
        int m_tickCount;
        bool m_paused;
        double m_pausedTimeAmount;
        double m_reversalTarget;
        double m_reversalSpeed;
        double m_currentTime;
        double m_previousTime;
        double m_startTime;
        double get_time();

    private:
#if defined(PLATFORM_WINDOWS)
        LARGE_INTEGER m_frequency;
#elif defined(PLATFORM_OSX)
        clock_serv_t m_cclock;
#endif
    };

    class FpsTimer: public Timer
    {
    public:
        FpsTimer();
        double get_fps();

    private:
        double m_fpsPreviousTime;
    };
} // namespace ORCore
