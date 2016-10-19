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

    int VorbisInput::getSampleRate() {
        return m_info->rate;
    }

    int VorbisInput::getChannelCount() {
        return m_info->channels;
    }

    double VorbisInput::getPosition() {
        return m_position;
    }

    int VorbisInput::readBuffer(float *pcm_channels, int sample_count) {
        int samplesSaved = 0;
        float **p_decodedSamples; // This will point on the decoded samples
        do {
            int samplesDecoded = ov_read_float(&m_vorbisFile, &p_decodedSamples,
                sample_count - samplesSaved, &currentSection);

            if (samplesDecoded < 0)
                throw std::runtime_error(errorCodeMap[samplesDecoded]);


            // TODO a solution valid for any channel count ?
            for (int i = 0; i < samplesDecoded; ++i) {
                pcm_channels[2*samplesSaved  ] = p_decodedSamples[0][i];
                pcm_channels[2*samplesSaved+1] = p_decodedSamples[1][i];
                samplesSaved++;
            }

            // End of file, stop asking for frames !
            if (samplesDecoded == 0) {
                m_eof = true;
                break;
            }
        } while (samplesSaved < sample_count);


        // ov_time_tell gives position of the next sample. So to be more accurate
        // we need to check this after the buffers are finished being read.
        m_position = ov_time_tell(&m_vorbisFile);
        return samplesSaved;
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
        m_logger->debug("Decoded length: {} samples", (long)ov_pcm_total(&myVorbisFile,-1));
        m_logger->debug("Encoded by: {}", ov_comment(&myVorbisFile,-1)->vendor);

    }
*/
