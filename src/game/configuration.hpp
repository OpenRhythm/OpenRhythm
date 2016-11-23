#pragma once
#include "configuration/parameter.hpp"

#define CONFIGURATION_FILE_NAME "OpenRhythm.yaml"


void readConfiguration(int argc, char** argv);
void writeConfigurationFile();
extern ORCore::Parameter<std::string> global_language;

extern ORCore::Parameter<std::string> path_library;
extern ORCore::Parameter<std::string> path_last_song;


extern ORCore::Parameter<std::pair<int,int>>    window_resolution;
extern ORCore::Parameter<int>                   window_samples;
extern ORCore::Parameter<bool>                  window_fullscreen;
extern ORCore::Parameter<std::string>           window_fps;
extern ORCore::Parameter<int>                   window_fps_max;


extern ORCore::Parameter<std::string> debug_song1;
extern ORCore::Parameter<std::string> debug_song2;
extern ORCore::Parameter<std::string> debug_midi1;
extern ORCore::Parameter<std::string> debug_midi2;
