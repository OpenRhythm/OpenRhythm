#pragma once
#include <string>
#include <vector>

namespace MgCore
{
    std::vector<std::string> splitString(const std::string str, const std::string delimiter, bool incDel = false);
}
