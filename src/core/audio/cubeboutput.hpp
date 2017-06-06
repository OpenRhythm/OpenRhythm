// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#pragma once
#include "streams.hpp"

#include "cubeb/cubeb.h"

namespace ORCore
{
    class CubebOutput: public Consumer
    {
    public:
        bool start();
        void stop();
        
        bool set_stream(Stream* stream)
        {
            m_stream = stream;
        }


    private:
        Stream* m_stream;
    };
}