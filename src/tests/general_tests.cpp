#include "os_utils.hpp"

#include <libintl.h>
#define _(STRING) gettext(STRING)


int main(int argc, char const *argv[]) {
    std::cout << get_config_directory() << std::endl;
    return 0;
}
