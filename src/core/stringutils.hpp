#pragma once
#include <string>
#include <vector>

namespace ORCore {
    int stringCount(const std::string& str, const std::string& substr);

    std::string stringJoin(const std::vector<std::string>& strElements, const std::string& delimiter);

    std::vector<std::string> stringSplit(const std::string& str, const std::string& delimiter);
} // namespace ORCore
