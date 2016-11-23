#include "config.hpp"
#include "soundio.hpp"

#include <stdexcept>


// TODO those defines should be runtime variables, dependant of the stream

// file dependant (now : uint16_t)
#define BYTES_PER_SAMPLE 4
// file dependent (now : stereo)
#define CHANNELS_COUNT 2

namespace ORCore {

    // The reason i seperated these from the class originally, is because they
    // are awful looking functions that clutter up the class definition, and they are really
    // just an implementation detail and they probably shouldn't be shown on documentation.
    static void write_callback_static(SoundIoOutStream *outStream, int frameCountMin, int frameCountMax) {
        auto stream = static_cast<SoundIoOutput*>(outStream->userdata);
        stream->write_callback(outStream, frameCountMin, frameCountMax);
    }
    static void underflow_callback_static(SoundIoOutStream *outStream) {
        auto stream = static_cast<SoundIoOutput*>(outStream->userdata);
        stream->underflow_callback(outStream);
    }

    SoundIoOutput::SoundIoOutput() {
        logger = spdlog::get("default");
        initialize();
    };

    void SoundIoOutput::initialize() {
        if (m_soundio) // Already initialized
            return;

        m_soundio = soundio_create();
        if (!m_soundio) {
            logger->error(_("SoundIO_create: out of memory"));
            throw std::runtime_error(_("SoundIO_create: out of memory"));
        }

        m_soundio->app_name = PROJECT_NAME;

        int err = soundio_connect(m_soundio);
        if (err) {
            logger->error(_("SoundIO_connect error: {}"), soundio_strerror(err));
            throw std::runtime_error(std::string(_("SoundIO_connect error: ")) + soundio_strerror(err));
        }

        soundio_flush_events(m_soundio);
    }

    SoundIoOutput::~SoundIoOutput() {
        close_stream();
        if (m_soundio != nullptr)
            soundio_destroy(m_soundio);
    }

    void SoundIoOutput::open_stream(int sample_rate, double latency, SoundIoFormat format) {
        if (m_device == nullptr) {
            throw std::runtime_error(_("Error while opening libsoundiostream : the device is not yet set !"));
        }

        m_outstream = soundio_outstream_create(m_device);
        m_outstream->format           = format;
        m_outstream->sample_rate      = sample_rate;
        m_outstream->software_latency = latency;

        // The userdata field is used to store the current instance so we
        // can call the correct class method when the callbacks are used.
        m_outstream->userdata             = this;

        m_outstream->write_callback       = &write_callback_static;
        m_outstream->underflow_callback   = &underflow_callback_static;

        m_outstream->name = PROJECT_NAME "SoundIoOutput";

        int err = soundio_outstream_open(m_outstream);
        if (err) {
            logger->error(_("unable to open device: {}"), soundio_strerror(err));
            throw std::runtime_error(std::string(_("unable to open device: ")) + soundio_strerror(err));
        }

        err = m_outstream->layout_error;
        if (err) {
            logger->error(_("unable to set channel layout: "), soundio_strerror(err));
            throw std::runtime_error(std::string(_("unable to set channel layout: ")) + soundio_strerror(err));
        }

        err = soundio_outstream_start(m_outstream);
        if (err) {
            logger->error(_("unable to start device: "), soundio_strerror(err));
            throw std::runtime_error(std::string(_("unable to start device: ")) + soundio_strerror(err));
        }
    }

    void SoundIoOutput::add_stream(AudioStream *stream) {
        m_AudioStreams.push_back(stream);
    }

    // Closes the stream
    void SoundIoOutput::close_stream() {
        if (m_outstream != nullptr) {
            soundio_outstream_destroy(m_outstream);
        }
    }

    void SoundIoOutput::connect_default_output_device() {
        if (m_device) // Already initialized
            return;

        int default_out_device_index = soundio_default_output_device_index(m_soundio);
        if (default_out_device_index < 0) {
            logger->error(_("SoundIO: No output device found"));
            throw std::runtime_error(_("SoundIO: No output device found"));
        }

        m_device = soundio_get_output_device(m_soundio, default_out_device_index);
        if (!m_device) {
            logger->error(_("SoundIO: out of memory"));
            throw std::runtime_error(_("SoundIO: out of memory"));
        }
    }

    void SoundIoOutput::disconnect_device() {
        soundio_device_unref(m_device);
    }

    SoundIoDevice* SoundIoOutput::get_device() {
        return m_device;
    };

    void SoundIoOutput::wait_events() {
        return soundio_wait_events(m_soundio);
    };

    void SoundIoOutput::write_callback(
        struct SoundIoOutStream *outStream, int frameCountMin, int frameCountMax) {

        const struct SoundIoChannelLayout *layout = &outStream->layout;
        struct SoundIoChannelArea *areas;

        int min = frameCountMax;
        for (auto stream: m_AudioStreams) {
            int framesProcessed = stream->process(frameCountMax);
            if (framesProcessed != 0)
                min = framesProcessed;
        }

        int err = soundio_outstream_begin_write(outStream, &areas, &min);
        if (err) {
            logger->error(_("{}"), soundio_strerror(err));
            throw std::runtime_error(_("SoundIO begin write failed"));
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

        for (auto stream: m_AudioStreams) {
            stream->cleanReadFrames(frameCountMax);
        }


        if ((err = soundio_outstream_end_write(outStream))) {
            logger->error(_("soundio error : {}\n"), soundio_strerror(err));
            throw std::runtime_error(_("SoundIO end write failed"));
        }
    }

    void SoundIoOutput::underflow_callback(SoundIoOutStream *outStream) {
        static int count = 0;
        logger->error(_("underflow {}\n"), count++);
    }

} // namespace ORCore
