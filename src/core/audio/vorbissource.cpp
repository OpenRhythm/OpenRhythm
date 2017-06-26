// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include <map>
#include <iostream>

#include "vorbissource.hpp"

namespace ORCore
{

    static std::map<int, std::string> errorCodeMap {
        {OV_HOLE,       "Vorbis: data interruption"},
        {OV_EBADLINK,   "Vorbis: Invalid Stream"},
        {OV_EINVAL,     "Vorbis: Invalid Headers"},
        {OV_EREAD,      "Vorbis: Read error."},
        {OV_ENOTVORBIS, "Vorbis: No vorbis data"},
        {OV_EVERSION,   "Vorbis: Version Mismatch"},
        {OV_EBADHEADER, "Vorbis: Invalid Vorbis bitstream header."},
        {OV_EFAULT,     "Vorbis: Internal logic fault."},
    };

    VorbisSource::VorbisSource(std::string filename)
    : m_filename(filename)
    {
        set_pause(false);
        int vorbisError = ov_fopen(m_filename.c_str(), &m_vorbisFile);
        if (vorbisError != 0)
        {
            throw std::runtime_error(errorCodeMap[vorbisError]);
        }

        m_info = ov_info(&m_vorbisFile,-1);

        // This is because some files do not seek to 0 automatically.
        if (ov_pcm_seek(&m_vorbisFile, 0) != 0)
        {
            throw std::runtime_error("Error seeking file to position 0.");
        }

    }


    VorbisSource::~VorbisSource()
    {
        ov_clear(&m_vorbisFile);
    }

    StreamFormat VorbisSource::get_format()
    {
        return {m_info->rate, m_info->channels};
    }


    void VorbisSource::pull(Buffer& buffer)
    {
        float **frameData = nullptr;
        int bitstream = -1;
        int framesRead = 0;
        auto bufferInfo = buffer.get_info();

        //std::cout << bufferInfo.frames << "\n";

        float *buff = buffer;

        while(framesRead < bufferInfo.frames)
        {
            // The buffer format returned from `ov_read_float` is not interleaved.
            // Its instead it is `buffer[channel][sample]`
            // This requires us to interleave the buffer ourselfs when copying
            // to the output buffer.

            // samplesRead is samples per channel read aka # of frames.

            // The vorbisfile documentation is lacking for this function... had to dig
            // into the source to figure this one out... :|
            long samplesRead = ov_read_float(&m_vorbisFile, &frameData, bufferInfo.frames - framesRead, &bitstream);

            if (samplesRead < 0)
            {
                throw std::runtime_error(errorCodeMap[samplesRead]);
            }

            // if we have reached the end of the file pause the stream.
            if (samplesRead == 0)
            {
                set_pause(true);
            }

            for (auto c = 0; c < m_info->channels; c++)
            {
                for (auto s = 0; s < samplesRead; s++)
                {
                    buff[framesRead+(samplesRead*bufferInfo.channels)+c] = frameData[c][s];
                }
            }
            framesRead += samplesRead;

            //std::cout << "samplesRead: " << samplesRead << " remain: " << (bufferInfo.frames - framesRead) << "\n";
        }
        set_time(ov_time_tell(&m_vorbisFile));
        
    }
}