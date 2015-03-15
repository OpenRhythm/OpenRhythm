#include "timing.hpp"

#include <iostream>
#include <iomanip>

namespace MgCore
{

    FpsTimer::FpsTimer()
    {

#if defined(PLATFORM_OSX)
        host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &m_cclock);
#endif
        m_frames = 1;
        m_currentTime = get_time();
        m_previousTime = m_currentTime;
        m_delta = 0.0;
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

    float FpsTimer::get_fps()
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