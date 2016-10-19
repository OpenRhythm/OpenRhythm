#include <spdlog/spdlog.h>

#include <iostream>
#include "core/audio/codecs/vorbis.hpp"
#include "core/audio/output/soundio.hpp"

#include <libintl.h>
#define _(STRING) gettext(STRING)

#include "config.hpp"


#define OggTestFile "TestOgg.ogg"



int main(int argc, char const *argv[]) {

    /* Setting the i18n environment */
    setlocale (LC_ALL, "");
    bindtextdomain ("openrhythm", LOCALE_DIR);
    textdomain ("openrhythm");

    /* Example of i18n usage */
    printf(_("Hello World\n"));

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
        std::cout << _("Logging Failed: ") << err.what() << std::endl;
        return 1;
    }


    std::string filename;

    ORCore::VorbisInput *mysong;
    try {
        mysong = new ORCore::VorbisInput(OggTestFile);
        mysong->open();
    } catch (const std::runtime_error& err) {
        std::cout << _("opening ogg vorbis file failed: ") << err.what() << std::endl;
    }

    std::cout << "####################" << std::endl;
    //mysong->getInfo();

    soundio_main(mysong);

}
