#ifndef VORBIS_HPP
#define VORBIS_HPP

#ifdef _WIN32
#   include <io.h>
#   include <fcntl.h>
#endif

#include <memory>
#include <string>
#include <vorbis/vorbisfile.h>

#include "spdlog/spdlog.h"
#include "audio/stream.hpp"

namespace ORCore {

    class VorbisInput: public AudioInputStream {
    public:

        // The default constructor
        // TODO: Integrate this with the VFS
        // @filename the absolute or relative file path
        VorbisInput(const std::string filename);
        // @inherit
        virtual int getBitDepth();
        // @inherit
        virtual int getChannelCount();
        // @inherit
        virtual int getSampleRate();
        // @inherit
        virtual double getPosition();
        // @inherit
        virtual void open();
        // @inherit
        virtual void close();
        // @inherit
        virtual int process(int frameCount);

    protected:
        std::shared_ptr<spdlog::logger> m_logger;

        // The filename (absolute or relative path)
        const std::string m_filename;
        double m_position;

        OggVorbis_File m_vorbisFile;
        vorbis_info *m_info;

        bool m_eof = false;
        int currentSection = -1;
    };

} // namespace ORCore
#endif  // VORBIS_HPP
