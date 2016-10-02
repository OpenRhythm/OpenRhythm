#ifndef STREAM_HPP
#define STREAM_HPP

#include <vector>

#include <iostream>

// Contains all channels of all samples of the buffer
// equiv. AudioBuffer[samplesCount][channelsCount]
using AudioBuffer = std::vector<float>;

// Channels order (Re = Rear, Si = Side)
// 1: Mono
// 2: Left   / Right
// 3: Left   / Center  / Right
// 4: FrLeft / FrRight / ReLeft  / ReRight
// 5: FrLeft / Center  / FrRight /  ReLeft / ReRight
// 6: FrLeft / Center  / FrRight /  ReLeft / ReRight / LFE
// 7: FrLeft / Center  / FrRight /  SiLeft / SiRight / ReCenter / LFE
// 8: FrLeft / Center  / FrRight /  SiLeft / SiRight / ReLeft   / ReRight / LFE


// This class defines the structure of an audio processing "task"
// such as resampling, filtering. It has an input and an output with samples
class AudioStream {
public:
    AudioStream() {};
    AudioStream(AudioStream *inputStream)
    : m_inputStream(inputStream) { };
   ~AudioStream() {};

    AudioBuffer *getFilledOutputBuffer() { return &m_outputBuffer; };

    virtual int process(int frameCount) = 0;
    // Should not change. Thus, defined in constructor ?
    virtual int getChannelCount() {
        return m_inputStream ?
               m_inputStream->getChannelCount() : 0;
    };

    void setAsInput(AudioStream *theInput) {
        m_inputStream = theInput;
    }


protected:
    AudioStream *m_inputStream;
    AudioBuffer *m_inputBuffer;
    AudioBuffer m_outputBuffer;


}; // class AudioStream

// An AudioInputStream is a special stream that does not have an input (decoder, generator,…)
class AudioInputStream : public AudioStream {
public:
    // Get the sample rate of the input
    virtual int getSampleRate() = 0;

    // Get the bit depth of the input
    virtual int getBitDepth() = 0;

    // Get the number of audio channels in the input
    virtual int getChannelCount() = 0;

    // Opens the input
    // Some heavy file access and decoding may occur here !
    virtual void open() = 0;

    // Closes the input
    // TODO add a bool to check if the file is open/closed
    virtual void close() = 0;

    // Returns the position inside the audio file
    // @return the value (in SOMETHING (seconds ?)) inside the audio file
    // TODO return a structure with more info ? (frame/time/total/…)
    virtual double getPosition() = 0;

    // todo: actually implement this.
    // virtual void musicHasFinished() = 0;
};

#endif // STREAM_HPP
