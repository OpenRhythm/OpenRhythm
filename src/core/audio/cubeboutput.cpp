// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "cubeboutput.hpp"

namespace ORCore
{

    CubebOutput::CubebOutput()
    :m_logger(spdlog::get("default"))
    {

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


    int CubebOutput::process(float* buffer, int frameCount)
    {
        return frameCount;
    }


    bool CubebOutput::start()
    {
        if (m_source == nullptr)
        {
            m_logger->error("Audio output source stream null.");
            return false;
        }
        StreamFormat format = m_source->get_format();

        cubeb_stream_params params;
        params.format = CUBEB_SAMPLE_FLOAT32NE; // Our entire audio pipeline will work in 32bit float.
        params.rate = format.sample_rate;
        params.channels = format.channels;
        params.layout = CUBEB_LAYOUT_STEREO;

        uint32_t frameLatency = 0;

        if (cubeb_get_min_latency(m_context, params, &frameLatency) != CUBEB_OK)
        {
            m_logger->error("cubeb: Failed to get minimum audio latency.");
            return false;
        }

        auto data_callback = [](cubeb_stream* stream, void* user_ptr,
            const void* input_buffer, void* output_buffer, long nframes) -> long
        {
            return static_cast<CubebOutput*>(user_ptr)->process(static_cast<float*>(output_buffer), nframes);
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

        return true;
    }

    void CubebOutput::stop()
    {
        cubeb_stream_destroy(m_stream);
    }
}