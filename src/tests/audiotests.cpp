#include "config.hpp"

#include <iostream>
#include <thread>

#include <spdlog/spdlog.h>

#include "core/audio/streams.hpp"
#include "core/audio/mixer.hpp"
#include "core/audio/vorbissource.hpp"
#include "core/audio/cubeboutput.hpp"

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
    //ORCore::Mixer mix;
    ORCore::CubebOutput out;

    // mix.add_source(&prod);

    out.set_source(&oggSource);
    out.start();

    // Wait 10 seconds then exit..
    std::this_thread::sleep_for(std::chrono::milliseconds(20000));
    oggSource.set_pause(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    oggSource.set_pause(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    out.stop();
}
