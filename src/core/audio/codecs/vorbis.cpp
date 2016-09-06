#include "vorbis.hpp"

#include <stdexcept>
#include <cstdio>
#include <iostream>
#include <map>
#include <string>

namespace ORCore {

    static std::map<int, std::string> errorCodeMap {
        {OV_HOLE, "Vorbis: data interruption"},
        {OV_EBADLINK, "Vorbis: Invalid Stream"},
        {OV_EINVAL, "Vorbis: Invalid Headers"},
        {OV_EREAD, "Vorbis: Read error."},
        {OV_ENOTVORBIS, "Vorbis: No vorbis data"},
        {OV_EVERSION, "Vorbis: Version Mismatch"},
        {OV_EBADHEADER, "Vorbis: Invalid Vorbis bitstream header."},
        {OV_EFAULT, "Vorbis: Internal logic fault."},
    };

    VorbisInput::VorbisInput(const std::string filename)
    : m_filename(filename)
    {
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

    int VorbisInput::readBuffer(char* buffer, int bufferSize) {
        int bytes;
        for (bytes = 0; bytes < bufferSize; ) {
            long bytesRead = ov_read(&m_vorbisFile, buffer, bufferSize, 0, 2, 1, &currentSection);

            if (bytesRead < 0) {
                throw std::runtime_error(errorCodeMap[bytesRead]);
            } else if (bytesRead == 0) {
                m_eof = true;
            }
            bytes += bytesRead;
        }

        // ov_time_tell gives position of the next sample. So to be more accurate
        // we need to check this after the buffers are finished being read.
        m_position = ov_time_tell(&m_vorbisFile);
        return bytes;
    }

} // namespace ORCore
