// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#pragma once
#include <string>

#include <vorbis/vorbisfile.h>

#include "streams.hpp"

namespace ORCore
{
    class VorbisSource: public ProducerStream
    {
    public:
        VorbisSource(std::string filename);
        ~VorbisSource();
        StreamFormat get_format();
        void do_pause();
        void pull(Buffer& buffer);

    private:
        std::string m_filename;

        OggVorbis_File m_vorbisFile;
        vorbis_info *m_info;
    };
}