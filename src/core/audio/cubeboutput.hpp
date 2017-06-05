#pragma once
#include "streams.hpp"

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