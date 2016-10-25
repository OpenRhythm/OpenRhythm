#include "configuration.hpp"

#include <libintl.h>
#define _(STRING) gettext(STRING)


// nameVisible, description,
// cliName, cliNameShort,
// valueDefault

Parameter<std::string> global_language(_("Language"), _("The language"),
    "", 0,
    "system");

Parameter<std::string> path_library( _(" "), _(" "),
    "", 0,
    "");
Parameter<std::string> path_last_song( _(" "), _(" "),
    "", 0,
    "");

Parameter<std::pair<int,int>> window_resolution( _("Window Resolution"), _(" "),
    "", 0,
    {640,480});
Parameter<int> window_samples( _(" "), _(" "),
    "", 0,
    4);
Parameter<bool> window_fullscreen( _(" "), _(" "),
    "", 0,
    false);
Parameter<std::string> window_fps( _(" "), _(" "),
    "", 0,
    "fixed");
Parameter<int> window_fps_max( _(" "), _(" "),
    "", 0,
    60);


