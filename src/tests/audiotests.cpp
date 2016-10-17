#include <spdlog/spdlog.h>

#include <iostream>
#include "core/audio/codecs/vorbis.hpp"
#include "core/audio/streams/resample.hpp"
#include "core/audio/output/soundio.hpp"

#include <chrono>
#include <thread>

#define OggTestFile "TestOgg.ogg"


int main(int argc, char const *argv[]) {
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


    // We should use a smart pointer raw new/delete is considered bad style nowdays.
    ORCore::VorbisInput *mysong;
    try {
        mysong = new ORCore::VorbisInput(OggTestFile);
        mysong->open();
    } catch (const std::runtime_error& err) {
        std::cout << "opening ogg vorbis file failed: " << err.what() << std::endl;
    }

    int songSampleRate = mysong->getSampleRate();
    int outputSampleRate = 44100;

    logger->info("SongSampleRate: {}", songSampleRate);


    auto soundOutput = new ORCore::SoundIoOutput();
    soundOutput->connect_default_output_device();
    soundOutput->open_stream_with_sample_rate(outputSampleRate);

    auto *resamplerstream =
        new ResamplerStream(mysong, SRC_SINC_MEDIUM_QUALITY);
    resamplerstream->setInputSampleRate(songSampleRate);
    resamplerstream->setOutputSampleRate(outputSampleRate);


    soundOutput->add_stream(resamplerstream);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    soundOutput->destroy();
    soundOutput->disconnect_device();


}
