#include "resample.hpp"

#include <iostream>

ResamplerStream::ResamplerStream(AudioStream *inputStream, int quality)
: AudioStream(inputStream) {
    int error = 0;
    m_src_state = src_new (quality, this->getChannelCount(), &error) ;
    if (!m_src_state) {
        throw std::runtime_error("Failed to init LibSampleRate");
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

void ResamplerStream::init() {
    // TODO remove as it may not be necessary after all
}

int ResamplerStream::process(int frameCount) {
    // Ask more frames to the input stream
    int inputFramesMax = frameCount * sampleRatio + 1;
    m_inputStream->process(inputFramesMax);
    m_outputBuffer.resize(frameCount * getChannelCount());

    m_src_data.data_in = m_inputStream->getFilledOutputBuffer()->data();
    m_src_data.data_out= m_outputBuffer.data();
    m_src_data.input_frames = inputFramesMax;
    m_src_data.output_frames= frameCount;
    m_src_data.end_of_input = 0;
    m_src_data.src_ratio    = 48000.0/44100.0;

    int src_process_state = src_process(m_src_state, &m_src_data);
    if (src_process_state != 0) {
        throw std::runtime_error(src_strerror(src_process_state));
    }

    return m_src_data.output_frames_gen;
}
