#include "timing.hpp"

#include <iostream>
#include <iomanip>

namespace MgCore
{

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