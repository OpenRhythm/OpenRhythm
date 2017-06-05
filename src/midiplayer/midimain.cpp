// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "config.hpp"

#include <iostream>
#include <vector>
#include <memory>
#include <iterator>
#include <spdlog/spdlog.h>


#include "MidiPlayer.hpp"



// Eventually we will want to load configuration files somewhere in here.
// This also means the VFS needs to be setup here as well
int main(int argc, char** argv)
{

    std::shared_ptr<spdlog::logger> logger;

    try {
        std::vector<spdlog::sink_ptr> sinks;
        // I would like to use the sink that does color output to the console
        // but it doesnt yet work for windows.
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
        // Probably will want to switch this to ostream_sink_mt once
        // the vfs is setup fully.
        sinks.push_back(std::make_shared<spdlog::sinks::simple_file_sink_mt>("midiplayer.log"));

        logger = std::make_shared<spdlog::logger>("default", std::begin(sinks), std::end(sinks));
        spdlog::register_logger(logger);

        // Should be set by the configuration eventually.
        logger->set_level(spdlog::level::info);

    } catch (const spdlog::spdlog_ex& err) {
        std::cout << _("Logging Failed: ") << err.what() << std::endl;
        return 1;
    }

    try {
        
        MidiPlayer::MidiDisplayManager mman;
        mman.start();

        logger->debug(_("Song loaded"));

    } catch (std::runtime_error &err) {
        logger->critical(_("Runtime Error:\n{}"), err.what());
        return 1;
    }
    return 0;
}
