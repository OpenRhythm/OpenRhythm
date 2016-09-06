
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "soundio.hpp"

// TODO those defines should be runtime variables, dependant of the stream

// file dependant
#define SAMPLE_RATE 44100
// file dependant (now : uint16_t)
#define BYTES_PER_SAMPLE 2
// file dependent (now : stereo)
#define CHANNELS_COUNT 2

namespace ORCore {
    // The global soundio instance
    struct SoundIo      *soundio = NULL;
    // The default device instance
    struct SoundIoDevice *device = NULL;

    static std::shared_ptr<spdlog::logger> logger; 

    int soundio_initialize() {
        if (soundio) // Already initialized
            return 0;

        logger = spdlog::get("default");

        soundio = soundio_create();
        if (!soundio) {
            logger->error("SoundIO_create: out of memory");
            throw std::runtime_error("SoundIO_create: out of memory");
        }

        soundio->app_name = "OpenRhythm";

        int err = soundio_connect(soundio);
        if (err) {
            logger->error("SoundIO_connect error: {}", soundio_strerror(err));
            throw std::runtime_error(std::string("SoundIO_connect error: ") + soundio_strerror(err));
        }

        soundio_flush_events(soundio);
        return 0;
    }

    int soundio_quit() {
        soundio_destroy(soundio);
        return 0;
    }

    int soundio_connect_default_output_device() {
        if (device) // Already initialized
            return 0;

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

        return 0;
    }

    int soundio_disconnect_device() {
        soundio_device_unref(device);
        return 0;
    }

    void write_callback(SoundIoOutStream *outstream, int frameCountMin, int frameCountMax) {
        auto stream = static_cast<SoundIoOStream*>(outstream->userdata);
        stream->write(outstream, frameCountMin, frameCountMax);
    }

    void underflow_callback(SoundIoOutStream *outstream) {
        auto stream = static_cast<SoundIoOStream*>(outstream->userdata);
        stream->underflow(outstream);
    }

    void SoundIoOStream::write(
        struct SoundIoOutStream *outstream, int frameCountMin, int frameCountMax) {

        const struct SoundIoChannelLayout *layout = &outstream->layout;
        struct SoundIoChannelArea *areas;
        int err;

        if ((err = soundio_outstream_begin_write(outstream, &areas, &frameCountMax))) {
            logger->error("{}", soundio_strerror(err));
            throw std::runtime_error("SoundIO begin write failed");
        }

        int byte_count_max = frameCountMax* BYTES_PER_SAMPLE*CHANNELS_COUNT;
        m_dataBuffer.resize(byte_count_max);

        theSong->readBuffer(reinterpret_cast<char*>(&m_dataBuffer[0]), byte_count_max);

        // Now we copy the data into the outstream !
        for (int i = 0; i < frameCountMax; ++i) {
            for (int channel = 0; channel < layout->channel_count; ++channel) {
                int16_t sample = m_dataBuffer[channel + 2*i];
                int16_t *ptr = (int16_t*)(areas[channel].ptr + areas[channel].step * i);
                *ptr = sample;
            }
        }

        if ((err = soundio_outstream_end_write(outstream))) {
            logger->error("soundio error : {}\n", soundio_strerror(err));
            throw std::runtime_error("SoundIO end write failed");
        }
    }

    void SoundIoOStream::underflow(SoundIoOutStream *outstream) {
        static int count = 0;
        logger->error("underflow {}\n", count++);
    }

    void SoundIoOStream::set_input(Input *theSong) {
        this->theSong = theSong;
    }

    int SoundIoOStream::open(double latency) {
        SoundIoFormat format;
        switch(theSong->getBitDepth()) {
            case  8: format = SoundIoFormatS8;
            case 16: format = SoundIoFormatS16NE;
            case 24: format = SoundIoFormatS24NE;
            case 32: format = SoundIoFormatS32NE;
            default: format = SoundIoFormatS16NE;

        }
        return open(format, theSong->getSampleRate(), latency);
    }

    int SoundIoOStream::open(SoundIoFormat format, int sample_rate, double latency) {
        outstream = soundio_outstream_create(device);
        outstream->format           = format;
        outstream->sample_rate      = sample_rate;
        outstream->software_latency = latency;

        // The userdata field is used to store the current instance so we
        // can call the correct class method when the callbacks are used.
        outstream->userdata             = this;

        outstream->write_callback       = &write_callback;
        outstream->underflow_callback   = &underflow_callback;

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
        return 0;
    }

    void SoundIoOStream::close() {
        soundio_outstream_destroy(outstream);
    }

    int soundio_main(Input *thesong) {

        soundio_initialize();
        soundio_connect_default_output_device();
        SoundIoOStream *mysoundioostream = new SoundIoOStream();
        mysoundioostream->set_input(thesong);
        mysoundioostream->open(0.01);

        for (;;)
            soundio_wait_events(soundio);

        delete(mysoundioostream);
        soundio_disconnect_device();
        soundio_quit();
        return 0;
    }


} // namespace ORCore
