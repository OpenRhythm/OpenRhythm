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
#include "audio/codec.hpp"

namespace FScore {

    class VorbisSong: public Codec {
    public:
        VorbisSong(const std::string filename)
        : Codec(filename) {
            m_logger = spdlog::get("default");
        };

        // @inherit
        void getInfo();
        // @inherit
        virtual int getSampleRate();
        // @inherit
        virtual int getBitDepth() { return 16; };
        // @inherit
        virtual int getChannelCount();
        // @inherit
        virtual double getPosition();
        // @inherit
        int open();
        // @inherit
        int close();

        int readBuffer(char* buffer, int bufferSize);


    protected:
        std::shared_ptr<spdlog::logger> m_logger;

        FILE *myFile;
        OggVorbis_File myVorbisFile;
        int eof=0;
        int current_section = 0;
    };

} // Namespace FScore

#endif  // VORBIS_HPP
