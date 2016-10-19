#include "vorbis.hpp"

#include <stdexcept>
#include <cstdio>
#include <iostream>
#include <map>
#include <string>

#include <libintl.h>
#define _(STRING) gettext(STRING)

namespace ORCore {

    static std::map<int, std::string> errorCodeMap {
        {OV_HOLE,       _("Vorbis: data interruption")},
        {OV_EBADLINK,   _("Vorbis: Invalid Stream")},
        {OV_EINVAL,     _("Vorbis: Invalid Headers")},
        {OV_EREAD,      _("Vorbis: Read error.")},
        {OV_ENOTVORBIS, _("Vorbis: No vorbis data")},
        {OV_EVERSION,   _("Vorbis: Version Mismatch")},
        {OV_EBADHEADER, _("Vorbis: Invalid Vorbis bitstream header.")},
        {OV_EFAULT,     _("Vorbis: Internal logic fault.")},
    };

    VorbisInput::VorbisInput(const std::string filename)
    : m_filename(filename) {
        m_logger = spdlog::get("default");
    }

    int VorbisInput::getBitDepth() {
        return 16;
    }
    int VorbisInput::getChannelCount() {
        return m_info->channels;
    }
    int VorbisInput::getSampleRate() {
        return m_info->rate;
    }

    void VorbisInput::open() {
        int vorbisError = ov_fopen(m_filename.c_str(), &m_vorbisFile);

        if (vorbisError != 0) {
            throw std::runtime_error(errorCodeMap[vorbisError]);
        }

        m_info = ov_info(&m_vorbisFile,-1);

        if (ov_pcm_seek(&m_vorbisFile, 0) != 0) {  // This is because some files do not seek to 0 automatically
            throw std::runtime_error(_("Error seeking file to position 0."));
        } else {
            m_logger->debug(_("Seeked file to position 0."));
        }
    }

    void VorbisInput::close() {
        ov_clear(&m_vorbisFile);
    }


    double VorbisInput::getPosition() {
        return m_position;
    }

    int VorbisInput::process(int frameCount) {
        float **p_decodedFrames; // This will point on the decoded frames
        while (m_framesInBuffer < frameCount) {
            int framesDecoded = ov_read_float(&m_vorbisFile, &p_decodedFrames,
                frameCount - m_framesInBuffer, &currentSection);

            if (framesDecoded < 0)
                throw std::runtime_error(errorCodeMap[framesDecoded]);


            for (int f = 0; f < framesDecoded; ++f) {
                for (int c = 0; c < getChannelCount(); ++c) {
                    m_outputBuffer.push_back(p_decodedFrames[c][f]);
                }
                m_framesInBuffer++;
            }

            // End of file, stop asking for frames !
            if (framesDecoded == 0) {
                m_eof = true;
                break;
            }
        }

        // ov_time_tell gives position of the next frame. So to be more accurate
        // we need to check this after the buffers are finished being read.
        m_position = ov_time_tell(&m_vorbisFile);
        return m_eof;
    }

} // namespace ORCore
