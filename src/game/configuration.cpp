#include "configuration.hpp"
#include "config.hpp"
#include <iostream>
#include <libintl.h>
#define _(STRING) gettext(STRING)

// valueDefault,
// nameVisible, description, cliName, cliNameShort,

Parameter<std::string>  global_language("system",
    _("Language"), _("The language"),
    "", 0);

Parameter<std::string>  path_library("",
    _(" "), _(" "), "", 0);
Parameter<std::string>  path_last_song("",
    _(" "), _(" "), "", 0);

Parameter<std::pair<int,int>> window_resolution({640,480},
    _("Window Resolution"), _(" "),
    "", 0);
Parameter<int>          window_samples(4,
    _(" "), _(" "), "", 0);
Parameter<bool>         window_fullscreen(false,
    _(" "), _(" "), "", 0);
Parameter<std::string>  window_fps("fixed",
    _(" "), _(" "), "", 0);
Parameter<int>          window_fps_max(60,
    _(" "), _(" "), "", 0);

Parameter<std::string>  debug_song1("",
    _(" "), _(" "), "", 0);
Parameter<std::string>  debug_song2("",
    _(" "), _(" "), "", 0);
Parameter<std::string>  debug_midi1("",
    _(" "), _(" "), "", 0);
Parameter<std::string>  debug_midi2("",
    _(" "), _(" "), "", 0);


template<typename T>
void setParam(Parameter<T> &parameter, YAML::Node node) {
    if(node.IsScalar()) {
        parameter.setConfigValue(node.as<T>());
    }
}

void readConfigurationFile(std::string path) {
    YAML::Node config;
    try {
        std::cout << _("Reading config file ") << path << "…";
        config = YAML::LoadFile(path);
        std::cout << std::endl;
    } catch(YAML::BadFile err) {
        std::cout << _(" does not exist !") << std::endl;
        return;
    }

    YAML::Node global = config["global"];
    setParam(global_language, global["language"]);

    YAML::Node paths  = config["paths"];
    setParam(path_library, paths["library"]);
    setParam(path_last_song, paths["last_song"]);

    YAML::Node window = config["window"];
    setParam(window_fps_max, window["fps_max"]);

    YAML::Node debug_songs = config["debug"]["test_songs"];
    if (debug_songs.IsSequence()){
         if(debug_songs.size()>=1)
            setParam(debug_song1, debug_songs[0]);
         if(debug_songs.size()>=2)
            setParam(debug_song1, debug_songs[1]);
    } else {
        std::cout << "debug: tests_songs:" << _(" is an invalid YAML sequence in ") << path << std::endl;
    }

    //…And so on.

}

void writeConfigurationFile(std::string path) {
    YAML::Emitter configFile;
    configFile
        << YAML::Comment("OpenRhythm Config")
        << YAML::BeginDoc
        << YAML::BeginMap

        << YAML::Key << "global"
            << YAML::BeginMap
            << YAML::Key << "language"
                << YAML::Value << global_language.getConfigValue()
            << YAML::EndMap

        << YAML::Key << "window"
            << YAML::BeginMap
            << YAML::Key << "resolution"
                << YAML::Value << YAML::Flow
                    << YAML::BeginSeq
                    << window_resolution.getConfigValue().first
                    << window_resolution.getConfigValue().second
                    << YAML::EndSeq
            << YAML::Key << "samples"
                << YAML::Value << window_samples.getConfigValue()
            << YAML::Key << "fullscreen"
                << YAML::Value << window_fullscreen.getConfigValue()
            << YAML::Key << "fps"
                << YAML::Value << window_fps.getConfigValue()
            << YAML::Key << "fps_max"
                << YAML::Value << window_fps_max.getConfigValue()
            << YAML::EndMap




        << YAML::EndMap

        << YAML::EndDoc;

    std::cout << "Here's the output YAML:\n" << configFile.c_str();
}



void readCommandLine() {

}

void readConfiguration() {
    readCommandLine();

    readConfigurationFile("data/default_config.yaml");
    // readConfigurationFile("custom_config.yaml");
    // readConfigurationFile(PACKAGE_DATA_DIR "/default_config.yaml");

}
