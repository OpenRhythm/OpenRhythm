#ifndef RESAMPLE_HPP
#define RESAMPLE_HPP
#include <samplerate.h>

#include "stream.hpp"

// enum SampleRateQuality {
//     SRC_SINC_BEST_QUALITY       = 0,
//     SRC_SINC_MEDIUM_QUALITY     = 1,
//     SRC_SINC_FASTEST            = 2,
//     SRC_ZERO_ORDER_HOLD         = 3,
//     SRC_LINEAR                  = 4
// };
#define DEFAULT_SAMPLERATE_QUALITY SRC_SINC_MEDIUM_QUALITY

class ResamplerStream: public AudioStream {
public:
    ResamplerStream() {};
    ~ResamplerStream() {
        src_delete(m_src_state);
    };


    void init();

protected:
    SRC_STATE* m_src_state = nullptr;

};


#endif // RESAMPLE_HPP
