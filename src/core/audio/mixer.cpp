// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "mixer.hpp"

namespace ORCore
{
    bool Mixer::add_stream(Stream* stream)
    {
        m_streams.push_back(stream);
        return true;
    }

    void Mixer::pull(Buffer& buffer)
    {
        buffer.clear();// clear output buffer
        float* buf = buffer;

        Buffer tempBuffer(buffer.get_info());
        float* tempbuf = buffer;


        // Go through all streams that are not paused and mix them into the output.
        for (auto &stream : m_streams)
        {
            if (!stream->is_paused())
            {
                stream->pull(tempBuffer);
                // DO MIXING HERE
                for (int i = 0; i < buffer.size(); ++i)
                {
                    buf[i] += tempbuf[i];
                }
            }
        }
    }

    StreamFormat Mixer::get_format()
    {
        // the format of the mixer will always be of the first stream that is passed into it.
        if (m_streams.size() > 0)
        {
            return m_streams[0]->get_format();
        }
    }
}