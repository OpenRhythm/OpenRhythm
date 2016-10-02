#include "vorbis.hpp"

#include <stdexcept>
#include <cstdio>
#include <iostream>
#include <map>
#include <string>

namespace ORCore {

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

    VorbisInput::VorbisInput(const std::string filename)
    : m_filename(filename) {
        m_logger = spdlog::get("default");
    }

    void VorbisInput::open() {
        int vorbisError = ov_fopen(m_filename.c_str(), &m_vorbisFile);

        if (vorbisError != 0) {
            throw std::runtime_error(errorCodeMap[vorbisError]);
        }

        m_info = ov_info(&m_vorbisFile,-1);

        if (ov_pcm_seek(&m_vorbisFile, 0) != 0) {  // This is because some files do not seek to 0 automatically
            throw std::runtime_error("Error seeking file to position 0.");
        } else {
            m_logger->debug("Seeked file to position 0.");
        }
    }

    void VorbisInput::close() {
        ov_clear(&m_vorbisFile);
    }

    int VorbisInput::getSampleRate() {
        return m_info->rate;
    }

    int VorbisInput::getChannelCount() {
        return m_info->channels;
    }

    double VorbisInput::getPosition() {
        return m_position;
    }

    int VorbisInput::process(int frameCount) {
        m_outputBuffer.resize(frameCount * getChannelCount());

        int framesSaved = 0;
        float **p_decodedFrames; // This will point on the decoded frames
        do {
            int framesDecoded = ov_read_float(&m_vorbisFile, &p_decodedFrames,
                frameCount - framesSaved, &currentSection);

            if (framesDecoded < 0)
                throw std::runtime_error(errorCodeMap[framesDecoded]);


            // TODO a solution valid for any channel count ?
            for (int i = 0; i < framesDecoded; ++i) {
                m_outputBuffer[framesSaved*getChannelCount() + 0] = p_decodedFrames[0][i];
                m_outputBuffer[framesSaved*getChannelCount() + 1] = p_decodedFrames[1][i];
                framesSaved++;
            }

            // End of file, stop asking for frames !
            if (framesDecoded == 0) {
                m_eof = true;
                break;
            }
        } while (framesSaved < frameCount);


        // ov_time_tell gives position of the next frame. So to be more accurate
        // we need to check this after the buffers are finished being read.
        m_position = ov_time_tell(&m_vorbisFile);
        return framesSaved;
    }

} // namespace ORCore
/*
    void VorbisSong::getInfo() {
        //char **ptr=ov_comment(&myVorbisFile,-1)->user_comments;
        // while(*ptr){
        //   fprintf(stderr,"%s\n",*ptr);
        //   ++ptr;
        // }

        vorbis_info *vi=ov_info(&myVorbisFile,-1);
        m_logger->debug("Bitstream is {} channel, {}Hz", vi->channels, vi->rate);
        m_logger->debug("Decoded length: {} frames", (long)ov_pcm_total(&myVorbisFile,-1));
        m_logger->debug("Encoded by: {}", ov_comment(&myVorbisFile,-1)->vendor);

    }
*/
