#pragma once
#if defined(PLATFORM_WINDOWS)
#   define SOUNDIO_STATIC_LIBRARY
#endif
#include <soundio/soundio.h>
#include "spdlog/spdlog.h"

#include "stream.hpp"

#define DEFAULT_SOUNDIO_SAMPLERATE  (48000)
#define DEFAULT_SOUNDIO_LATENCY     (0.010)
//#define DEFAULT_SOUNDIO_FORMAT      (SoundIoFormatS16NE)
#define DEFAULT_SOUNDIO_FORMAT      (SoundIoFormatFloat32NE)

namespace ORCore {
    // Singleton class describing the libSoundIO output
    class SoundIoOutput {
    public:
        SoundIoOutput();
       ~SoundIoOutput();

        // Opens a stream using the current output device, sample rate and latency
        void open_stream(int sample_rate = DEFAULT_SOUNDIO_SAMPLERATE,
                         double latency = DEFAULT_SOUNDIO_LATENCY,
                         SoundIoFormat format = DEFAULT_SOUNDIO_FORMAT);
        void open_stream_with_sample_rate(int sample_rate) {
             open_stream(sample_rate, DEFAULT_SOUNDIO_LATENCY, DEFAULT_SOUNDIO_FORMAT);
        }
        void open_stream_with_latency(double latency) {
            open_stream(DEFAULT_SOUNDIO_SAMPLERATE, latency,   DEFAULT_SOUNDIO_FORMAT);
        }

        void add_stream(AudioStream *stream);

        // Closes the stream
        void close_stream();
        void destroy() {
            soundio_destroy(m_soundio);
        }
        void flush_events() {
            soundio_flush_events(m_soundio);
        }


        // To call at the start of the app. Initializes *m_device.
        // @return 0 if correctly initialized, 1 if there was an error.
        void connect_default_output_device();
        void disconnect_device();

        SoundIoDevice* get_device();
        void wait_events();

        void write_callback(SoundIoOutStream *outStream, int frameCountMin, int frameCountMax);
        void underflow_callback(SoundIoOutStream *outStream);

    protected:

        // Called in the SoundIoOutput constructor
        // @throws runtime_errors on error
        void initialize();

        // The unique libSoundIO instance
        SoundIo             *m_soundio = nullptr;
        // The unique output stream for libSoundIO
        SoundIoOutStream    *m_outstream;
        // The unique device instance for libSoundIO
        SoundIoDevice       *m_device = nullptr;

        // The buffer to be sent to the libsoundio backend
        std::vector<float> m_dataBuffer;

        // Contains all the streams (song, sounds,…) to play together
        std::vector<AudioStream*> m_AudioStreams;

        // The spdlogger global instance
        std::shared_ptr<spdlog::logger> logger;
    };


} // namespace ORCore
