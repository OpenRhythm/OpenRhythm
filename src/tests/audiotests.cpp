#include <spdlog/spdlog.h>

#include <iostream>
#include "core/audio/codecs/vorbis.hpp"
#include "core/audio/output/soundio.hpp"


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


    std::string filename;

    ORCore::VorbisInput *mysong;
    try {
        mysong = new ORCore::VorbisInput(OggTestFile);
        mysong->open();
    } catch (const std::runtime_error& err) {
        std::cout << "opening ogg vorbis file failed: " << err.what() << std::endl;
    }

    std::cout << "####################" << std::endl;
    //mysong->getInfo();


    auto soundOutput = new ORCore::SoundIoOutput();
    soundOutput->connect_default_output_device();
    soundOutput->open_stream();

    // We should use a smart pointer raw new/delete is considered bad style nowdays.
    auto *mysoundioostream = new ORCore::AudioOutputStream();
    mysoundioostream->set_input(mysong);

    soundOutput->add_stream(mysoundioostream);

    for (;;)
        soundOutput->wait_events();

    delete(mysoundioostream);
    soundOutput->disconnect_device();


}
