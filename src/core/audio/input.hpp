#ifndef INPUT_HPP
#define INPUT_HPP

#include <string>

namespace ORCore {

    // The interface for inputs into the audio pipeline.

    class Input {
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

        // Writes into <buffer> up to <buffersize> bytes of decoded audio
        // TODO - convert bytes to float32
        // @return the number of actually decodec bytes
        virtual int readBuffer(char* buffer, int bufferSize) = 0;

        // Returns the position inside the audio file
        // @return the value (in SOMETHING (seconds ?)) inside the audio file
        // TODO return a structure with more info ? (frame/time/total/…)
        virtual double getPosition() = 0;

        // todo: actually implement this.
        // virtual void musicHasFinished() = 0;
    };

} // namespace ORCore
#endif // INPUT_HPP
