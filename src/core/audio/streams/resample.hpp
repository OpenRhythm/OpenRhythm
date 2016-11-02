#pragma once
#include <samplerate.h>

#include "stream.hpp"

namespace ORCore {

    // enum SampleRateQuality {
    //     SRC_SINC_BEST_QUALITY       = 0,
    //     SRC_SINC_MEDIUM_QUALITY     = 1,
    //     SRC_SINC_FASTEST            = 2,
    //     SRC_ZERO_ORDER_HOLD         = 3,
    //     SRC_LINEAR                  = 4
    // };

    #define DEFAULT_SAMPLERATE_QUALITY SRC_SINC_MEDIUM_QUALITY
    #define DEFAULT_SAMPLERATE_SAMPLERATE 44100.0

    class ResamplerStream: public AudioStream {
    public:
        ResamplerStream(AudioStream *inputStream, int quality);
        ResamplerStream(AudioStream *inputStream)
        : ResamplerStream(inputStream, DEFAULT_SAMPLERATE_QUALITY) {}
        ~ResamplerStream();

        void setInputSampleRate(double samplerate_in);
        void setOutputSampleRate(double samplerate_out);

        int process(int frameCount);

    protected:
        double samplerate_in = DEFAULT_SAMPLERATE_SAMPLERATE;
        double samplerate_out= DEFAULT_SAMPLERATE_SAMPLERATE;
        double sampleRatio = samplerate_out / samplerate_in;

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

} // namespace ORCore
