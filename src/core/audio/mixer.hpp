#pragma once
#include <vector>

#include "streams.hpp"

namespace ORCore
{
    class Mixer: public InputStream
    {
    public:
        bool add_stream(Stream* stream);
        void pull(Buffer& buffer);
        StreamFormat get_format();

        // Mixer will never be paused.
        bool is_paused()
        {
            return false;
        }


    private:
        std::vector<Stream*> m_streams;
    };
}