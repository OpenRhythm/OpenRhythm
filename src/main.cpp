// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "config.hpp"

#include <iostream>
#include <vector>
#include <memory>
#include <iterator>
#include <SDL.h>
#include <spdlog/spdlog.h>

#include "game.hpp"

// Eventually we will want to load configuration files somewhere in here.
int main(int argc, char** argv)
{

// TODO - Move this to into a function/class within a future i18n module.
// TODO - Plans here are to not use gettext and either use an alternative more cross platform
// alternative or make our own/somewhere in the middle.
// Here is a few random alternatives i've found:
// https://github.com/kaishiqi/I18N-Gettext-Supported
//    - Haven't dug into much, MIT licensed cross platform, uses mo files.
// https://github.com/tinygettext/tinygettext
//    - tbh the code is pretty bad, and if we were going to use this it would only be as a beginning.
//    - Uses po files.
// https://github.com/laurent22/simple-gettext
//    - GPL Licensed, mo file parser.
// https://github.com/clawoo/mofilereader
//    - MIT License, mo file parser. Only supports UTF-8 (Totally fine with me)
// https://github.com/vslavik/poedit
//    - Useful translation editor.
#if defined(TRANSLATION_ENABLED)
    // Setting the i18n environment
    setlocale (LC_ALL, "");
    bindtextdomain ("openrhythm", LOCALE_DIR);
    textdomain ("openrhythm");
#endif

    // Example of i18n usage
    printf(_("Hello World\n"));

    std::shared_ptr<spdlog::logger> logger;

    try {
        std::vector<spdlog::sink_ptr> sinks;
        // I would like to use the sink that does color output to the console
        // but it doesnt yet work for windows.
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
        // Probably will want to switch this to ostream_sink_mt once
        // the vfs is setup fully.
        sinks.push_back(std::make_shared<spdlog::sinks::simple_file_sink_mt>("openrhythm.log"));

        logger = std::make_shared<spdlog::logger>("default", std::begin(sinks), std::end(sinks));
        spdlog::register_logger(logger);

        // Should be set by the configuration eventually.
        logger->set_level(spdlog::level::info);

    } catch (const spdlog::spdlog_ex& err) {
        std::cout << _("Logging Failed: ") << err.what() << std::endl;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, _("Runtime Error"), err.what(), nullptr);
        return 1;
    }

    try {
        ORGame::GameManager game;
        game.start();
    } catch (std::runtime_error &err) {
        logger->critical(_("Runtime Error:\n{}"), err.what());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, _("Runtime Error"), err.what(), nullptr);
        return 1;
    }
    return 0;
}
