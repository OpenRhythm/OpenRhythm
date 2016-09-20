#ifndef STREAM_HPP
#define STREAM_HPP

#include <vector>

// Containing all channels for a sample
using AudioSample = std::vector<float>;
// Containing all samples for a buffer
using AudioBuffer = std::vector<AudioSample>;

inline
AudioBuffer createAudioBuffer(int channelCount) {
    return AudioBuffer(0, AudioSample(channelCount));
}

// Channels order (Re = Rear, Si = Side)
// 1: Mono
// 2: Left   / Right
// 3: Left   / Center  / Right
// 4: FrLeft / FrRight / ReLeft  / ReRight
// 5: FrLeft / Center  / FrRight /  ReLeft / ReRight
// 6: FrLeft / Center  / FrRight /  ReLeft / ReRight / LFE
// 7: FrLeft / Center  / FrRight /  SiLeft / SiRight / ReCenter / LFE
// 8: FrLeft / Center  / FrRight /  SiLeft / SiRight / ReLeft   / ReRight / LFE


class AudioStream {
public:
    AudioStream() {};
    AudioStream(AudioStream *inputStream) {};
   ~AudioStream() {};

   AudioBuffer* getFilledOutputBuffer() { return &outputBuffer; };


   // Should not change. Thus, defined in constructor ?
   int getChannelCount() { return inputStream->getChannelCount(); };

protected:
    AudioStream *inputStream;
    AudioBuffer *inputBuffer;
    AudioBuffer outputBuffer;


};

#endif // STREAM_HPP
