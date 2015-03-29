#include <iostream>
#include "utils.hpp"

namespace MgCore
{
    std::vector<std::string> splitString(const std::string str, const std::string delimiter, bool incDel)
    {
        std::vector<std::string> split;

        std::size_t start = 0;
        std::size_t end = 0;
        bool running = true;

        while (running) {
            end = str.find(delimiter, start);
            if (end == std::string::npos) {
                end = str.size();
                running = false;
            }
            if (incDel == true and start != 0) {
                start -= delimiter.size();
            }
            split.push_back(str.substr(start, end-start));
            start = end + delimiter.size();
        }

        return split;
    }
}