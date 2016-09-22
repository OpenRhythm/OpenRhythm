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

void ResamplerStream::init() {

}

void ResamplerStream::process(int frameCount) {
    //reinterpret_cast<const byte *>

    // Ask more frames to the input stream

//    int inFramesCount = inputStream->m_outputBuffer;
}
