#include "config.hpp"
#include "resample.hpp"
#include <fmt/format.h>

#include <stdexcept>

namespace ORCore {

    ResamplerStream::ResamplerStream(AudioStream *inputStream, int quality)
    : AudioStream(inputStream) {
        int error = 0;
        m_src_state = src_new(quality, this->getChannelCount(), &error);
        if (!m_src_state) {
            throw std::runtime_error(fmt::format("Failed to init LibSampleRate: {}", src_strerror(error)));
        }
    }

    ResamplerStream::~ResamplerStream() {
        src_delete(m_src_state);
    }

    void ResamplerStream::setInputSampleRate(double samplerate_in_) {
        samplerate_in = samplerate_in_;
        sampleRatio = samplerate_out / samplerate_in;
    }
    void ResamplerStream::setOutputSampleRate(double samplerate_out_) {
        samplerate_out = samplerate_out_;
        sampleRatio = samplerate_out / samplerate_in;
    }

    int ResamplerStream::process(int frameCount) {

        // Get a big enough buffer
        m_outputBuffer.resize(frameCount * getChannelCount());

        while (m_framesInBuffer < frameCount) {
            // Ask more frames to the input stream
            int inputFrameCount = (frameCount - m_framesInBuffer) * sampleRatio + 1;
            m_inputStream->process(inputFrameCount);

            m_src_data.data_in = m_inputStream->getFilledOutputBuffer()->data();
            m_src_data.input_frames = inputFrameCount;

            m_src_data.data_out= m_outputBuffer.data() + m_framesInBuffer*getChannelCount();
            m_src_data.output_frames= frameCount - m_framesInBuffer;

            m_src_data.end_of_input = 0;
            m_src_data.src_ratio    = sampleRatio;

            int error = src_process(m_src_state, &m_src_data);
            if (error != 0) {
                throw std::runtime_error(fmt::format("Failed to init LibSampleRate: {}", src_strerror(error)));
            }

            m_framesInBuffer +=            m_src_data.output_frames_gen;
            m_inputStream->cleanReadFrames(m_src_data.input_frames_used);

        }

        return m_src_data.output_frames_gen;
    }


} // namespace ORCore
