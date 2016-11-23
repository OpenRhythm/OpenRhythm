#include <iostream>
#include <yaml-cpp/yaml.h>

#include "config.hpp"
#include "configuration/parameter.hpp"
#include "configuration.hpp"


int main(int argc, char const *argv[]) {
    //std::cout << get_config_directory() << std::endl;
    readConfiguration();
    std::cout << path_library.getValue() << std::endl;;




    writeConfigurationFile();
    return 0;
}
