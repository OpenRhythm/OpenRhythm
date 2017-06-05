// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "vorbissource.hpp"

namespace ORCore
{
    VorbisSource::VorbisSource(std::string filename)
    : m_filename(filename)
    {

    }

    StreamFormat VorbisSource::get_format()
    {
        return {0, 0};
    }


    void VorbisSource::pull(Buffer& buffer)
    {
        
    }
}