#include <iostream>
#include <yaml-cpp/yaml.h>
#include <libintl.h>
#define _(STRING) gettext(STRING)

#include "configuration/parameter.hpp"
#include "configuration.hpp"


int main(int argc, char const *argv[]) {
    //std::cout << get_config_directory() << std::endl;
    readConfiguration();
    std::cout << debug_song1.getValue() << std::endl;;




    writeConfigurationFile("");
    return 0;
}
