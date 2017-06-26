#include "config.hpp"

#include <iostream>
#include <thread>

#include <spdlog/spdlog.h>

#include "core/audio/streams.hpp"
#include "core/audio/mixer.hpp"
#include "core/audio/vorbissource.hpp"
#include "core/audio/cubeboutput.hpp"

class SineStream: public ORCore::ProducerStream 
{ 
public:
    SineStream(int frequency)
    :m_frequency(frequency)
    {
        set_pause(false);
    }
    ORCore::StreamFormat get_format() 
    { 
        return {44100, 2}; 
    } 

    void pull(ORCore::Buffer& buffer) 
    { 
        float *buf = buffer;
        auto bufferInfo = buffer.get_info();
        for (auto i = 0; i < bufferInfo.frames; i++) 
        {
            float sample = sin(2*3.14159265 * (i + m_framePosition) * m_frequency/44100) * 0.125;
            for (auto c = 0; c < bufferInfo.channels; c++)
            {
                buf[(i*bufferInfo.channels)+c] = sample;
            } 
        } 
        m_framePosition += buffer.get_info().frames; 
    } 
private:
    int m_frequency;
    int m_framePosition = 0; 

}; 

int main() {

    std::shared_ptr<spdlog::logger> logger;

    try {
        std::vector<spdlog::sink_ptr> sinks;
        // I would like to use the sink that does color output to the console
        // but it doesnt yet work for windows.
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
        // Probably will want to switch this to ostream_sink_mt once
        // the vfs is setup fully.
        sinks.push_back(std::make_shared<spdlog::sinks::simple_file_sink_mt>("openrhythmaudio.log"));

        logger = std::make_shared<spdlog::logger>("default", std::begin(sinks), std::end(sinks));
        spdlog::register_logger(logger);

        // Should be set by the configuration eventually.
        logger->set_level(spdlog::level::debug);

    } catch (const spdlog::spdlog_ex& err) {
        std::cout << "Logging Failed: " << err.what() << std::endl;
        return 1;
    }
    ORCore::VorbisSource oggSource("song.ogg");

    ORCore::Mixer mix;
    SineStream sine(350);
    SineStream sine2(440);
    ORCore::CubebOutput out;

    mix.add_source(&sine);
    mix.add_source(&sine2);
    mix.add_source(&oggSource);

    out.set_source(&mix);
    out.start();

    // Wait 10 seconds then exit..
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    sine.set_pause(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    sine.set_pause(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    out.stop();
}
