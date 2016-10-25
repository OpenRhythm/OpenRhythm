#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <map>

#include "configuration/parameter.hpp"


extern Parameter<std::string> global_language;

extern Parameter<std::string> path_library;
extern Parameter<std::string> path_last_song;


extern Parameter<std::pair<int,int>>    window_resolution;
extern Parameter<int>                   window_samples;
extern Parameter<bool>                  window_fullscreen;
extern Parameter<std::string>           window_fps;
extern Parameter<int>                   window_fps_max;

#endif // CONFIGURATION_HPP
