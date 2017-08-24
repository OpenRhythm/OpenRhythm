// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.


#include <cstdarg>

#include "cubeboutput.hpp"

namespace ORCore
{
    void print_log(const char * msg, ...)
    {
      va_list args;
      va_start(args, msg);
      vprintf(msg, args);
      va_end(args);
    }

    CubebOutput::CubebOutput()
    :m_logger(spdlog::get("default"))
    {
        cubeb_set_log_callback(CUBEB_LOG_DISABLED, print_log);

        if (cubeb_init(&m_context, "OpenRhythm", nullptr) != CUBEB_OK)
        {
            m_logger->error("cubeb: Audio initialization failed.");
        }
    }

    CubebOutput::~CubebOutput()
    {
        cubeb_destroy(m_context);
    }

    void CubebOutput::set_source(Stream* stream)
    {
        m_source = stream;
    }


    void CubebOutput::build_buffer(Buffer& audioBuffer)
    {
        if (!m_source->is_paused())
        {
            m_source->pull(audioBuffer);
        }
        else
        {
            audioBuffer.clear();
        }
    }


    bool CubebOutput::start()
    {
        if (m_source == nullptr)
        {
            m_logger->error("Audio output source stream null.");
            return false;
        }
        m_format = m_source->get_format();
        cubeb_channel_layout lay;

        cubeb_get_preferred_channel_layout(m_context, &lay);


        m_logger->info("defult: {} picked: {}", lay, CUBEB_LAYOUT_STEREO);


        cubeb_stream_params params;
        params.format = CUBEB_SAMPLE_FLOAT32NE; // Our entire audio pipeline will work in 32bit float.
        params.rate = m_format.sampleRate;
        params.channels = m_format.channels;
        params.layout = CUBEB_LAYOUT_STEREO;


        uint32_t rate;

        cubeb_get_preferred_sample_rate(m_context, &rate);

        m_logger->info("defult: {} picked: {}", rate, m_format.sampleRate);

        uint32_t frameLatency = 0;

        if (cubeb_get_min_latency(m_context, &params, &frameLatency) != CUBEB_OK)
        {
            m_logger->error("cubeb: Failed to get minimum audio latency.");
            return false;
        }

        auto data_callback = [](cubeb_stream* stream, void* user_ptr,
            const void* input_buffer, void* output_buffer, long nframes) -> long
        {
            auto* cubebOut = static_cast<CubebOutput*>(user_ptr);
            Buffer audioBuffer(static_cast<float*>(output_buffer), {cubebOut->m_format.channels, static_cast<int>(nframes)});
            cubebOut->build_buffer(audioBuffer);
            return nframes;
        };

        auto state_callback = [](cubeb_stream* stream, void* user_ptr, cubeb_state state) -> void
        {
        };

        if (cubeb_stream_init(m_context, &m_stream, "OpenRhythm Main Output", nullptr, nullptr, nullptr,
            &params, frameLatency, data_callback, state_callback, this) != CUBEB_OK)
        {
            m_logger->error("cubeb: Failed to initialize stream.");
            return false;
        }

        if (cubeb_stream_start(m_stream) != CUBEB_OK)
        {
            m_logger->error("cubeb: Failed to start stream.");
            return false;
        }

        return true;
    }

    void CubebOutput::stop()
    {
        if (cubeb_stream_stop(m_stream) != CUBEB_OK)
        {
            m_logger->error("cubeb: Failed to stop stream.");
        }
        cubeb_stream_destroy(m_stream);
    }
}
