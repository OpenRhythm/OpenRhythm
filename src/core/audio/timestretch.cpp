// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include <iostream>
#include "timestretch.hpp"

namespace ORCore
{

    TimeStretch::TimeStretch()
    {
        m_soundtouchInstance = soundtouch_create();
        m_speed.store(0.0, std::memory_order_release);
        m_speed.store(false, std::memory_order_release);
    }

    TimeStretch::~TimeStretch()
    {
        soundtouch_destroy(m_soundtouchInstance);
    }


    void TimeStretch::set_speed(float speed)
    {
        m_speedChanged.store(true, std::memory_order_release);
        m_speed.store(speed, std::memory_order_release);
    }

    bool TimeStretch::add_source(Stream* stream)
    {
        m_stream = stream;

        StreamFormat format = m_stream->get_format();
        
        // Setup format specific soundtouch info.
        soundtouch_setChannels(m_soundtouchInstance, format.channels);
        soundtouch_setSampleRate(m_soundtouchInstance, format.sampleRate);
        soundtouch_clear(m_soundtouchInstance); // In case there was a previous stream.

        return true;
    }

    void TimeStretch::pull(Buffer& buffer)
    {
        buffer.clear();// clear output buffer
        float* buf = buffer;

        Buffer tempBuffer(buffer.get_info());
        tempBuffer.clear();
        float* tempbuf = tempBuffer;

        float speedChanged = m_speedChanged.load(std::memory_order_acquire);

        if (speedChanged)
        {
            float speed = m_speed.load(std::memory_order_acquire) * 100.0f;
            soundtouch_setTempoChange(m_soundtouchInstance, speed);
        }

        // We want to only pull samples if the stream isn't paused.
        if (!m_stream->is_paused())
        {
            do
            {
                m_stream->pull(tempBuffer);
                soundtouch_putSamples(m_soundtouchInstance, tempbuf, static_cast<unsigned int>(tempBuffer.size()));
                std::cout << soundtouch_numSamples(m_soundtouchInstance) << " " << soundtouch_numUnprocessedSamples(m_soundtouchInstance) << std::endl;
            } while(soundtouch_numSamples(m_soundtouchInstance) < static_cast<unsigned int>(buffer.size()));

            soundtouch_receiveSamples(m_soundtouchInstance, buf, static_cast<unsigned int>(buffer.size()));
        }
    }

    StreamFormat TimeStretch::get_format()
    {
        return m_stream->get_format();
    }
}