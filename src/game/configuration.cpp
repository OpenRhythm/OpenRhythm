#include "config.hpp"
#include "configuration.hpp"
#include <iostream>
#include <fstream>

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


Parameter<int>  audio_bits(16,
    _(" "), _(" "), "", 0);
Parameter<int>  audio_framerate(44100,
    _(" "), _(" "), "", 0);
Parameter<int>  audio_latency_ms(10,
    _(" "), _(" "), "", 0);
Parameter<bool>  audio_stereo(true,
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
            setParam(debug_song2, debug_songs[1]);
    } else {
        std::cout << "debug: tests_songs:" << _(" is an invalid YAML sequence in ") << path << std::endl;
    }

    //…And so on.

}

void writeConfigurationFile() {
    YAML::Emitter configFile;
    configFile
    << YAML::Comment("OpenRhythm Config")
    << YAML::BeginDoc
    << YAML::BeginMap

    << YAML::Key << "global"
        << YAML::BeginMap
        << YAML::Key << "language"      << YAML::Value << global_language
        << YAML::EndMap

    << YAML::Key << "paths"
        << YAML::BeginMap
        << YAML::Key << "library"       << YAML::Value << path_library
        << YAML::Key << "last_song"     << YAML::Value << path_last_song
        << YAML::EndMap

    << YAML::Key << "window"
        << YAML::BeginMap
        << YAML::Key << "resolution"    << YAML::Flow  << YAML::BeginSeq
            << window_resolution.getConfigValue().first
            << window_resolution.getConfigValue().second
            << YAML::EndSeq
        << YAML::Key << "samples"       << YAML::Value << window_samples
        << YAML::Key << "fullscreen"    << YAML::Value << window_fullscreen
        << YAML::Key << "fps"           << YAML::Value << window_fps
        << YAML::Key << "fps_max"       << YAML::Value << window_fps_max
        << YAML::EndMap

    << YAML::Key << "interface"
        << YAML::BeginMap
        << YAML::Key << "theme"         << YAML::Value << "MegaLight GH3"
        << YAML::Key << "note_rotate"   << YAML::Value << true
        << YAML::Key << "neck"          << YAML::Value << "defaultneck"
        << YAML::EndMap

    << YAML::Key << "audio"
        << YAML::BeginMap
        << YAML::Key << "backend"
            << YAML::BeginMap
            << YAML::Key << "bits"      << YAML::Value << audio_bits
            << YAML::Key << "framerate" << YAML::Value << audio_framerate
            << YAML::Key << "latency_ms"<< YAML::Value << audio_latency_ms
            << YAML::Key << "stereo"    << YAML::Value << audio_stereo
            << YAML::EndMap
        << YAML::Key << "volumes"
            << YAML::BeginMap
            << YAML::Key << "track"     << YAML::Value << 100
            << YAML::Key << "background"<< YAML::Value << 80
            << YAML::Key << "screw-up"  << YAML::Value << 40
            << YAML::Key << "miss"      << YAML::Value << 20
            << YAML::Key << "crowd"     << YAML::Value << 80
            << YAML::Key << "kill"      << YAML::Value << 00
            << YAML::Key << "effects"   << YAML::Value << 70
            << YAML::Key << "menu"      << YAML::Value << 60
            << YAML::EndMap
        << YAML::EndMap

    << YAML::Key << "menus"
        << YAML::BeginMap
        << YAML::Key << "audio_preview" << YAML::Value << true
        << YAML::Key << "preview_delay" << YAML::Value << 100
        << YAML::EndMap

    << YAML::Key << "game"
        << YAML::BeginMap
        << YAML::Key << "music_delay_ms"<< YAML::Value << 0
        << YAML::Key << "enable_crowd"  << YAML::Value << true
        << YAML::Key << "mute_end_secs" << YAML::Value << 0
        << YAML::Key << "default_speed" << YAML::Value << 1
        << YAML::Key << "whammy_effect" << YAML::Value << false
        << YAML::Key << "hit_window"    << YAML::Value << 0
        << YAML::EndMap

    << YAML::Key << "debug"
        << YAML::BeginMap
        << YAML::Key << "show_fps"      << YAML::Value << true
        << YAML::Key << "show_bpm"      << YAML::Value << false
        << YAML::Key << "show_overlay"  << YAML::Value << false
        << YAML::Key << "show_raw_vocal"<< YAML::Value << false
        << YAML::Key << "log_level"     << YAML::Value << "debug"
        << YAML::Key << "test_songs"    << YAML::BeginSeq
            << debug_song1
            << debug_song2
            << YAML::EndSeq
        << YAML::Key << "test_midis"    << YAML::BeginSeq
            << debug_midi1
            << debug_midi2
            << YAML::EndSeq
        << YAML::EndMap

    << YAML::Key << "users"
        << YAML::BeginSeq;
    // for (User user : users) {
    for (int i = 0; i < 1; ++i) {
        configFile
            << YAML::BeginMap
            << YAML::Key << "name"          << YAML::Value << "Salamandar"
            << YAML::Key << "date-created"  << YAML::Value << "2016-10-18"
            << YAML::EndMap;
    }
    configFile
        << YAML::EndSeq

    << YAML::Key << "controllers"
        << YAML::BeginMap
        << YAML::Key << "guitars"
        << YAML::BeginSeq;
    // for (Guitar guitar : guitars) {
    for (int i = 0; i < 1; ++i) {
        configFile
            << YAML::BeginMap
            << YAML::Key << "name"          << YAML::Value << "Default Guitar"
            << YAML::Key << "id"            << YAML::Value << i
            << YAML::Key << "notes"         << YAML::Flow  << YAML::BeginSeq
                << "F1"
                << "F2"
                << "F3"
                << "F4"
                << "F5"
                << YAML::EndSeq
            << YAML::Key << "solos"         << YAML::Flow  << YAML::BeginSeq
                << "F1"
                << "F2"
                << "F3"
                << "F4"
                << "F5"
                << YAML::EndSeq
            << YAML::Key << "picks"         << YAML::Flow  << YAML::BeginSeq
                << "Return"
                << "Right Shift"
                << YAML::EndSeq
            << YAML::Key << "star"          << YAML::Value << "Right Ctrl"
            << YAML::Key << "whammy"        << YAML::Value << "None"
            << YAML::Key << "menus"
                << YAML::BeginMap
                << YAML::Key << "left"          << YAML::Value << "Left"
                << YAML::Key << "right"         << YAML::Value << "Right"
                << YAML::Key << "up"            << YAML::Value << "Up"
                << YAML::Key << "down"          << YAML::Value << "Down"
                << YAML::Key << "cancel"        << YAML::Value << "Escape"
                << YAML::Key << "kill"          << YAML::Value << "PageUp"
                << YAML::EndMap
            << YAML::EndMap;
    }
    configFile
        << YAML::EndSeq

        << YAML::Key << "microphones"
        << YAML::BeginSeq;
    // for (Microphone microphone : microphone) {
    for (int i = 0; i < 1; ++i) {
        configFile
            << YAML::BeginMap
            << YAML::Key << "name"          << YAML::Value << "Default Guitar"
            << YAML::Key << "id"            << YAML::Value << i
            << YAML::EndMap;
    }
    configFile
        << YAML::EndSeq
        << YAML::EndMap



    << YAML::EndMap
    << YAML::EndDoc;

    std::ofstream fout(CONFIGURATION_FILE_NAME);
    fout << configFile.c_str();
}



void readCommandLine() {

}

void readConfiguration() {
    readCommandLine();

    readConfigurationFile("data/default_config.yaml");
    // readConfigurationFile(PACKAGE_DATA_DIR "/default_config.yaml" CONFIGURATION_FILE_NAME);
    readConfigurationFile(CONFIGURATION_FILE_NAME);
}