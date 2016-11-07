#pragma once
#include "configuration/parameter.hpp"

#define CONFIGURATION_FILE_NAME "OpenRhythm.yaml"

using namespace ORCore;

void readConfiguration(int argc, char** argv);
void writeConfigurationFile();
extern Parameter<std::string> global_language;

extern Parameter<std::string> path_library;
extern Parameter<std::string> path_last_song;


extern Parameter<std::pair<int,int>>    window_resolution;
extern Parameter<int>                   window_samples;
extern Parameter<bool>                  window_fullscreen;
extern Parameter<std::string>           window_fps;
extern Parameter<int>                   window_fps_max;


extern Parameter<std::string> debug_song1;
extern Parameter<std::string> debug_song2;
extern Parameter<std::string> debug_midi1;
extern Parameter<std::string> debug_midi2;
