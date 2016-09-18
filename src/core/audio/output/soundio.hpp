#ifndef SOUNDIO_HPP
#define SOUNDIO_HPP

#include <soundio/soundio.h>
#include "spdlog/spdlog.h"

#include "input.hpp"

#define DEFAULT_SOUNDIO_SAMPLERATE  (44100)
#define DEFAULT_SOUNDIO_LATENCY     (0.1)
//#define DEFAULT_SOUNDIO_FORMAT      (SoundIoFormatS16NE)
#define DEFAULT_SOUNDIO_FORMAT      (SoundIoFormatFloat32NE)

namespace ORCore {
    class AudioOutputStream;

    // Singleton class describing the libSoundIO output
    class SoundIoOutput {
    public:
        SoundIoOutput();
       ~SoundIoOutput();

        // Opens a stream using the current output device, sample rate and latency
        void open_stream(SoundIoFormat format = DEFAULT_SOUNDIO_FORMAT,
                         int sample_rate = DEFAULT_SOUNDIO_SAMPLERATE,
                         double latency = DEFAULT_SOUNDIO_LATENCY);

        void add_stream(AudioOutputStream *stream);

        // Closes the stream
        void close_stream();


        // To call at the start of the app. Initializes *device.
        // @return 0 if correctly initialized, 1 if there was an error.
        void connect_default_output_device();
        void disconnect_device();

        SoundIoDevice* get_device();
        void wait_events();

        void write_callback(SoundIoOutStream *outstream, int frameCountMin, int frameCountMax);
        void underflow_callback(SoundIoOutStream *outstream);

    protected:

        // Called in the SoundIoOutput constructor
        // @throws runtime_errors on error
        void initialize();

        // The unique libSoundIO instance
        SoundIo             *soundio = nullptr;
        // The unique output stream for libSoundIO
        SoundIoOutStream    *outstream;
        // The unique device instance for libSoundIO
        SoundIoDevice       *device = nullptr;

        // The buffer to be sent to the libsoundio backend
        std::vector<float> m_dataBuffer;

        // Contains all the streams (song, sounds,…) to play together
        std::vector<AudioOutputStream*> m_AllStreams;

        // The spdlogger global instance
        std::shared_ptr<spdlog::logger> logger;
    };



    class AudioOutputStream {
    public:
        AudioOutputStream() {};
        ~AudioOutputStream() {};

        void set_input(Input *thesong);

        void read(int frameCountMax);

        std::vector<float> *get_buffer();

    protected:
        Input* theSong;

        // The buffer to be sent to the libsoundio backend
        std::vector<float> m_dataBuffer;

        std::shared_ptr<spdlog::logger> logger;
    };

    // A test function
    int soundio_main(Input *thesong);

} // namespace ORCore
#endif // SOUNDIO_HPP
