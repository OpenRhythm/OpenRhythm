#ifndef SOUNDIO_HPP
#define SOUNDIO_HPP

#include <memory>
#include "spdlog/spdlog.h"

#include <soundio/soundio.h>

#include "input.hpp"

#define DEFAULT_SOUNDIO_LATENCY (0.1)

namespace ORCore {

    // To call at the start of the app. Initializes *soundio.
    // @return 0 if correctly initialized, 1 if there was an error.
    int soundio_initialize();
    int soundio_quit();

    // To call at the start of the app. Initializes *device.
    // @return 0 if correctly initialized, 1 if there was an error.
    int soundio_connect_default_output_device();
    int soundio_disconnect_device();

    void write_callback(SoundIoOutStream *outstream, int frameCountMin, int frameCountMax);
    void underflow_callback(SoundIoOutStream *outstream);

    class SoundIoOStream {
    public:
        SoundIoOStream() {};
        ~SoundIoOStream() {};

        void set_input(Input *thesong);

        int open(SoundIoFormat format, int sample_rate, double latency);
        int open(SoundIoFormat format, int sample_rate)
        { return open(format, sample_rate, DEFAULT_SOUNDIO_LATENCY); }
        int open(double latency);

        void write(SoundIoOutStream *outstream, int frameCountMin, int frameCountMax);
        void underflow(SoundIoOutStream *outstream);

        void close();


    protected:
        Input* theSong;
        SoundIoOutStream *outstream;

        std::vector<int16_t> m_dataBuffer;

    };

    // A test function
    int soundio_main(Input *thesong);

} // namespace ORCore
#endif // SOUNDIO_HPP
