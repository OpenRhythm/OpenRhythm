#include "vorbis.hpp"

#include <stdexcept>
#include <stdlib.h>
#include <iostream>

namespace ORCore {

    int VorbisSong::open() {
        myFile = fopen(this->filename.c_str(), "rb");
        if (!myFile)
            return -1;

        int open_ret = ov_open_callbacks(myFile, &myVorbisFile, NULL, 0, OV_CALLBACKS_NOCLOSE);

        switch (open_ret) {
            case OV_EREAD:       // A read from media returned an error.
            case OV_ENOTVORBIS:  // Bitstream does not contain any Vorbis data.
            case OV_EVERSION:    // Vorbis version mismatch.
            case OV_EBADHEADER:  // Invalid Vorbis bitstream header.
            case OV_EFAULT:      // Internal logic fault; indicates a bug or heap/stack corruption.
                m_logger->error("Input does not appear to be an Ogg bitstream.");
                fclose(myFile);
                throw std::runtime_error("Not an ogg bitstream");
                break;
            case 0:
                break;
        }

        if (ov_pcm_seek(&myVorbisFile, 0) != 0){  // This is because some files do not seek to 0 automatically
            m_logger->error("Error seeking file to position 0.");
            throw std::runtime_error("Error seeking file to position 0.");
        } else
            m_logger->debug("Seeked file to position 0.");
        return 0;
    }

    int VorbisSong::close() {
        ov_clear(&myVorbisFile);
        return 0;
    }

    int VorbisSong::getSampleRate() {
        return ov_info(&myVorbisFile,-1)->rate;
    }

    int VorbisSong::getChannelCount() {
        return ov_info(&myVorbisFile,-1)->channels;
    }

    double VorbisSong::getPosition() {
        return ov_time_tell(&myVorbisFile);
    }

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

    int VorbisSong::readBuffer(char* buffer, int bufferSize) {
        long read_ret = ov_read(&myVorbisFile, buffer, bufferSize,
            0, 2, 1, &current_section);

        switch(read_ret) {
            case 0:         // indicates EOF
                eof = 1;
                m_logger->debug("eof of {}", filename);
                return 1;
                break;
            case OV_HOLE:
                    // indicates there was an interruption in the data.
                    // (one of: garbage between pages, loss of sync followed by recapture, or a corrupt page)
            case OV_EBADLINK:
                    // indicates that an invalid stream section was supplied to libvorbisfile, or the requested link is corrupt.
            case OV_EINVAL:
                    // indicates the initial file headers couldn't be read or are corrupt, or that the initial open call for vf failed.
                // There was an error, TL;DR
                m_logger->error("vorbis_read_error: {}", read_ret);
                return 0;
                break;
            default:
                return read_ret;
        }
    }

} // namespace ORCore
