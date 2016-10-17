#include "soundio.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <vector>

#include "config.hpp"

#include "chrono.hpp"

// TODO those defines should be runtime variables, dependant of the stream

// file dependant (now : uint16_t)
#define BYTES_PER_SAMPLE 4
// file dependent (now : stereo)
#define CHANNELS_COUNT 2

namespace ORCore {

    // The reason i seperated these from the class originally, is because they
    // are awful looking functions that clutter up the class definition, and they are really
    // just an implementation detail and they probably shouldn't be shown on documentation.
    static void write_callback_static(SoundIoOutStream *outstream, int frameCountMin, int frameCountMax) {
        auto stream = static_cast<SoundIoOutput*>(outstream->userdata);
        stream->write_callback(outstream, frameCountMin, frameCountMax);
    }
    static void underflow_callback_static(SoundIoOutStream *outstream) {
        auto stream = static_cast<SoundIoOutput*>(outstream->userdata);
        stream->underflow_callback(outstream);
    }

    SoundIoOutput::SoundIoOutput() {
        logger = spdlog::get("default");
        initialize();
    };

    void SoundIoOutput::initialize() {
        if (soundio) // Already initialized
            return;

        soundio = soundio_create();
        if (!soundio) {
            logger->error("SoundIO_create: out of memory");
            throw std::runtime_error("SoundIO_create: out of memory");
        }

        soundio->app_name = PROJECT_NAME;

        int err = soundio_connect(soundio);
        if (err) {
            logger->error("SoundIO_connect error: {}", soundio_strerror(err));
            throw std::runtime_error(std::string("SoundIO_connect error: ") + soundio_strerror(err));
        }

        soundio_flush_events(soundio);
    }

    SoundIoOutput::~SoundIoOutput() {
        close_stream();
        if (soundio != nullptr)
            soundio_destroy(soundio);
    }

    void SoundIoOutput::open_stream(int sample_rate, double latency, SoundIoFormat format) {
        if (device == nullptr) {
            throw std::runtime_error("Error while opening libsoundiostream : the device is not yet set !");
        }

        outstream = soundio_outstream_create(device);
        outstream->format           = format;
        outstream->sample_rate      = sample_rate;
        outstream->software_latency = latency;

        // The userdata field is used to store the current instance so we
        // can call the correct class method when the callbacks are used.
        outstream->userdata             = this;

        outstream->write_callback       = &write_callback_static;
        outstream->underflow_callback   = &underflow_callback_static;

        outstream->name = PROJECT_NAME "SoundIoOutput";

        int err = soundio_outstream_open(outstream);
        if (err) {
            logger->error("unable to open device: {}", soundio_strerror(err));
            throw std::runtime_error(std::string("unable to open device: ") + soundio_strerror(err));
        }

        err = outstream->layout_error;
        if (err) {
            logger->error("unable to set channel layout: ", soundio_strerror(err));
            throw std::runtime_error(std::string("unable to set channel layout: ") + soundio_strerror(err));
        }

        err = soundio_outstream_start(outstream);
        if (err) {
            logger->error("unable to start device: ", soundio_strerror(err));
            throw std::runtime_error(std::string("unable to start device: ") + soundio_strerror(err));
        }
    }

    void SoundIoOutput::add_stream(AudioStream *stream) {
        m_AudioStreams.push_back(stream);
    }

    // Closes the stream
    void SoundIoOutput::close_stream() {
        if (outstream != nullptr) {
            soundio_outstream_destroy(outstream);
        }
    }

    void SoundIoOutput::connect_default_output_device() {
        if (device) // Already initialized
            return;

        int default_out_device_index = soundio_default_output_device_index(soundio);
        if (default_out_device_index < 0) {
            logger->error("SoundIO: No output device found");
            throw std::runtime_error("SoundIO: No output device found");
        }

        device = soundio_get_output_device(soundio, default_out_device_index);
        if (!device) {
            logger->error("SoundIO: out of memory");
            throw std::runtime_error("SoundIO: out of memory");
        }
    }

    void SoundIoOutput::disconnect_device() {
        soundio_device_unref(device);
    }

    SoundIoDevice* SoundIoOutput::get_device() {
        return device;
    };

    void SoundIoOutput::wait_events() {
        return soundio_wait_events(soundio);
    };

    void SoundIoOutput::write_callback(
        struct SoundIoOutStream *outstream, int frameCountMin, int frameCountMax) {
        // Timer bench_timer;

        const struct SoundIoChannelLayout *layout = &outstream->layout;
        struct SoundIoChannelArea *areas;

        int min = frameCountMax;
        for (auto stream: m_AudioStreams) {
            int framesProcessed = stream->process(frameCountMax);
            if (framesProcessed != 0)
                min = framesProcessed;
        }

        int err = soundio_outstream_begin_write(outstream, &areas, &min);
        if (err) {
            logger->error("{}", soundio_strerror(err));
            throw std::runtime_error("SoundIO begin write failed");
        }

        m_dataBuffer.resize(frameCountMax*layout->channel_count);

        // Now we copy the data into the outstream !
        for (int i = 0; i < frameCountMax; ++i) {
            for (int channel = 0; channel < layout->channel_count; ++channel) {
                float *ptr = (float*)(areas[channel].ptr + areas[channel].step * i);
                *ptr = 0;
                for (auto stream: m_AudioStreams) {
                    float sample = stream->getFilledOutputBuffer()
                        ->at(channel + layout->channel_count * i);
                    *ptr += sample;
                }
                // Pass tanh() to the samples to remove possible overflows
                // due to decoding and mixing
                *ptr = tanh(*ptr);
            }
        }

        if ((err = soundio_outstream_end_write(outstream))) {
            logger->error("soundio error : {}\n", soundio_strerror(err));
            throw std::runtime_error("SoundIO end write failed");
        }
        // std::cout << "time in audio callback (max/real): "
        //           << frameCountMax/48000.0 << " "
        //           << std::fixed << bench_timer.finish() << std::endl;
    }

    void SoundIoOutput::underflow_callback(SoundIoOutStream *outstream) {
        static int count = 0;
        logger->error("underflow {}\n", count++);
    }

} // namespace ORCore
