#include "resample.hpp"

#include <iostream>

ResamplerStream::ResamplerStream(int quality) {
    int error = 0;
    m_src_state = src_new (quality, this->getChannelCount(), &error) ;
    if (!m_src_state) {
        throw std::runtime_error("Failed to init LibSampleRate");
    }
}
ResamplerStream::ResamplerStream() {
    ResamplerStream(DEFAULT_SAMPLERATE_QUALITY);
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

void ResamplerStream::init() {
    // TODO remove as it may not be necessary after all
}

void ResamplerStream::process(int frameCount) {
    // Ask more frames to the input stream
    int inputFramesMax = frameCount * sampleRatio + 1;
    m_inputStream->process(inputFramesMax);
    m_outputBuffer.resize(frameCount);

    m_src_data.data_in = reinterpret_cast<float*>(m_inputStream->getFilledOutputBuffer().data());
    m_src_data.data_out= reinterpret_cast<float*>(m_outputBuffer.data());
    m_src_data.input_frames = inputFramesMax;
    m_src_data.output_frames= frameCount;
    m_src_data.end_of_input = 0;
    m_src_data.src_ratio    = sampleRatio;


    int src_process_state = src_process(m_src_state, &m_src_data);

    if (src_process_state != 0) {
        throw std::runtime_error(src_strerror(src_process_state));
    }

    // m_src_data.input_frames_used
    // m_src_data.output_frames_gen

}
