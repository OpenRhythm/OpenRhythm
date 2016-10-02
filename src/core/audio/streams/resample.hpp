#ifndef RESAMPLE_HPP
#define RESAMPLE_HPP
#include <samplerate.h>

#include "output/soundio.hpp"
#include "stream.hpp"

// enum SampleRateQuality {
//     SRC_SINC_BEST_QUALITY       = 0,
//     SRC_SINC_MEDIUM_QUALITY     = 1,
//     SRC_SINC_FASTEST            = 2,
//     SRC_ZERO_ORDER_HOLD         = 3,
//     SRC_LINEAR                  = 4
// };

#define DEFAULT_SAMPLERATE_QUALITY SRC_SINC_MEDIUM_QUALITY
#define DEFAULT_SAMPLERATE_SAMPLERATE 44100

class ResamplerStream: public AudioStream {
public:
    ResamplerStream(int quality);
    ResamplerStream();
    ~ResamplerStream();

    void setInputSampleRate(double samplerate_in);
    void setOutputSampleRate(double samplerate_out);

    void init();
    void process(int frameCount);

protected:
    double samplerate_in = DEFAULT_SAMPLERATE_SAMPLERATE;
    double samplerate_out= DEFAULT_SAMPLERATE_SAMPLERATE;
    double sampleRatio = 1;

    SRC_STATE* m_src_state = nullptr;
    SRC_DATA   m_src_data = {
        nullptr,// *data_in,
        nullptr,// *data_out ;
        0,      // input_frames,
        0,      // output_frames ;
        0,      // input_frames_used,
        0,      // output_frames_gen ;
        0,      // end_of_input ;
        1,      // src_ratio ;
    };

};


#endif // RESAMPLE_HPP
