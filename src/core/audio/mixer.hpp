// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#pragma once
#include <vector>

#include "streams.hpp"

namespace ORCore
{
    class Mixer: public InputStream
    {
    public:
        bool add_source(Stream* stream);
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