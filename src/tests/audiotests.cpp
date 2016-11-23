#include <spdlog/spdlog.h>

#include <iostream>
#include <chrono>
#include <thread>
#include "core/audio/codecs/vorbis.hpp"
#include "core/audio/streams/resample.hpp"
#include "core/audio/output/soundio.hpp"

#include "configuration.hpp"

#include "config.hpp"

int main(int argc, char *argv[]) {

// TODO - Move this to into a function/class within a future i18n module.
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
        sinks.push_back(std::make_shared<spdlog::sinks::simple_file_sink_mt>("openrhythmaudio.log"));

        logger = std::make_shared<spdlog::logger>("default", std::begin(sinks), std::end(sinks));
        spdlog::register_logger(logger);

        // Should be set by the configuration eventually.
        logger->set_level(spdlog::level::debug);

    } catch (const spdlog::spdlog_ex& err) {
        std::cout << _("Logging Failed: ") << err.what() << std::endl;
        return 1;
    }

    // Read configuration
    readConfiguration(argc, argv);
    std::string OggTestFile     = debug_song1.getValue();
    std::string OggAnotherFile  = debug_song2.getValue();



    // You may change that to change your config
    int outputSampleRate = 44100;

    // Initialize the audio output
    auto soundOutput = new ORCore::SoundIoOutput();
    soundOutput->connect_default_output_device();
    soundOutput->open_stream_with_sample_rate(outputSampleRate);


    // First ogg file
    try {
        ORCore::VorbisInput *mysong = new ORCore::VorbisInput(OggTestFile);
        mysong->open();

        int songSampleRate = mysong->getSampleRate();

        auto *resamplerstream =
            new ORCore::ResamplerStream(mysong, SRC_SINC_MEDIUM_QUALITY);
        resamplerstream->setInputSampleRate(songSampleRate);
        resamplerstream->setOutputSampleRate(outputSampleRate);

        logger->info(_("SongSampleRate: {}"), songSampleRate);

        logger->debug(_("addstream resamplerstream"));
        soundOutput->add_stream(resamplerstream);
    } catch (const std::runtime_error& err) {
        std::cout << _("opening ogg vorbis file failed: ") << err.what() << std::endl;
    }

ltiple streams output
    ORCore::VorbisInput *anotherOgg;
    try {
        logger->debug("add audio file \"{}\"", OggAnotherFile);


        ORCore::VorbisInput *anotherOgg = new ORCore::VorbisInput(OggAnotherFile);
        anotherOgg->open();

        int anotherOggSampleRate = anotherOgg->getSampleRate();

        auto *anotherResamplerstream =
            new ORCore::ResamplerStream(anotherOgg, SRC_SINC_MEDIUM_QUALITY);
        anotherResamplerstream->setInputSampleRate(anotherOggSampleRate);
        anotherResamplerstream->setOutputSampleRate(outputSampleRate);

        soundOutput->add_stream(anotherResamplerstream);
    } catch (const std::runtime_error& err) {
        std::cout << _("opening ogg vorbis file failed: ") << err.what() << std::endl;
    }



    // Wait a while then stop all that
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    soundOutput->destroy();
    soundOutput->disconnect_device();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

}
