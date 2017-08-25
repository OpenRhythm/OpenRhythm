// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#pragma once
#include <vector>
#include <atomic>

#include <soundtouch-c.h>

#include "streams.hpp"


namespace ORCore
{
    class TimeStretch: public InputStream
    {
    public:
        TimeStretch();
        ~TimeStretch();
        void set_speed(float speed);
        bool add_source(Stream* stream);
        void pull(Buffer& buffer);
        StreamFormat get_format();

        // Effect will never be paused.
        bool is_paused()
        {
            return false;
        }


    private:
        Stream* m_stream;
        ST_INSTANCE m_soundtouchInstance;

        std::atomic<float> m_speed;
        std::atomic_bool m_speedChanged;
    };
}