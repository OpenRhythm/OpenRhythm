#include "timing.hpp"

#include <iostream>
#include <iomanip>

namespace MgCore
{
    FpsTimer::FpsTimer()
    {
        host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &m_cclock);
    }

    FpsTimer::~FpsTimer()
    {
        mach_port_deallocate(mach_task_self(), m_cclock);
    }

    double FpsTimer::tick()
    {
        

        m_previousTime = m_currentTime;
        m_currentTime = get_time();

        m_delta = m_currentTime - m_previousTime;
        m_fpsTime += m_delta;
        m_frames++;
        return m_fpsTime;

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