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
        static SoundIoOutput& getInstance() {
            static SoundIoOutput instance;  // Guaranteed to be destroyed.
                                            // Instantiated on first use.
            return instance;
        }
        SoundIoOutput (SoundIoOutput const&)    = delete;
        void operator=(SoundIoOutput const&)    = delete;



        // Opens a stream using the current output device, sample rate and latency
        void open_stream(SoundIoFormat format = DEFAULT_SOUNDIO_FORMAT,
                         int sample_rate = DEFAULT_SOUNDIO_SAMPLERATE,
                         double latency = DEFAULT_SOUNDIO_LATENCY);
        // Closes the stream
        void close_stream() {
            if (outstream != nullptr)
                soundio_outstream_destroy(outstream);
        }


        // To call at the start of the app. Initializes *device.
        // @return 0 if correctly initialized, 1 if there was an error.
        void connect_default_output_device();
        void disconnect_device();

        struct SoundIoDevice* get_device() { return device; };

        void wait_events() { return soundio_wait_events(soundio); };


        static
        void write_callback_static
            (SoundIoOutStream *outstream, int frameCountMin, int frameCountMax);
        void write_callback
            (SoundIoOutStream *outstream, int frameCountMin, int frameCountMax);

        static
        void underflow_callback_static(SoundIoOutStream *outstream);
        void underflow_callback       (SoundIoOutStream *outstream);

        void add_stream(AudioOutputStream *stream) {
            m_AllStreams.push_back(stream);
        }

    protected:
        SoundIoOutput() {
            logger = spdlog::get("default");
            initialize();
        };
       ~SoundIoOutput() {
            close_stream();
            if (soundio != nullptr)
                soundio_destroy(soundio);
        }

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

        std::vector<float> *get_buffer() {
            return &m_dataBuffer;
        }


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
