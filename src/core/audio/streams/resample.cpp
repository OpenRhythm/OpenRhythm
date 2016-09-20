#include "resample.hpp"

void ResamplerStream::init() {
    int error = 0;
    m_src_state = src_new (DEFAULT_SAMPLERATE_QUALITY, this->getChannelCount(), &error) ;
    if (!m_src_state) {
        throw std::runtime_error("Failed to init LibSampleRate");
    }

}
