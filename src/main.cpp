#include <iostream>
#include <vector>
#include <memory>
#include <iterator>
#include <SDL.h>
#define SPDLOG_NO_NAME
#define SPDLOG_NO_THREAD_ID // Might be useful to enable later but atm meh
#include <spdlog/spdlog.h>

#include <libintl.h>
#define _(STRING) gettext(STRING)

#include "game.hpp"
#include "config.hpp"

// Eventually we will want to load configuration files somewhere in here.
// This also means the VFS needs to be setup here as well
int main()
{
    // Setting the i18n environment
    setlocale (LC_ALL, "");
    bindtextdomain ("openrhythm", LOCALE_DIR);
    textdomain ("openrhythm");

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
        GameManager game;
        game.start();
    } catch (std::runtime_error &err) {
        logger->critical(_("Runtime Error:\n{}"), err.what());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, _("Runtime Error"), err.what(), nullptr);
        return 1;
    }
    return 0;
}
