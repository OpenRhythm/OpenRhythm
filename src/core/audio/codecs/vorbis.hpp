#ifndef VORBIS_HPP
#define VORBIS_HPP

#include <string>
#include <vorbis/vorbisfile.h>

#include "audio/codec.hpp"

#ifdef _WIN32
#   include <io.h>
#   include <fcntl.h>
#endif

namespace FScore {

    class VorbisSong: public Codec {
    public:
        VorbisSong(const std::string filename)
        : Codec(filename) {};

        // @inherit
        void getInfo();
        // @inherit
        int open();
        // @inherit
        int close();

        int readBuffer(char* buffer, int bufferSize);


    protected:
        FILE *myFile;
        OggVorbis_File myVorbisFile;
        int eof=0;
        int current_section = 0;
    };

} // Namespace FSCore

#endif  // VORBIS_HPP
