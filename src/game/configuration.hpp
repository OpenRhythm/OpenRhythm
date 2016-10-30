#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "configuration/parameter.hpp"


#define CONFIGURATION_FILE_NAME "OpenRhythm.yaml"



void readConfiguration();
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

#endif // CONFIGURATION_HPP
