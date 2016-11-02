#include "stringutils.hpp"

namespace ORCore
{
    int stringCount(const std::string& str, const std::string& substr)
    {
        std::size_t start = 0;
        int count = 0;
        while (true){
            start = str.find(substr, start);
            if (start != std::string::npos) {
                count++;
                start += substr.size();
            } else {
                break;
            }
        }

        return count;
    }

    std::string stringJoin(const std::vector<std::string>& strElements, const std::string& delimiter)
    {
        int strSize = 0;
        for (const auto &str : strElements)
        {
            strSize += str.size();
        }
        std::string output;
        output.reserve(strSize);
        const std::string &vec_back = strElements.back();
        // int vecSize = strElements.size();
        for (const auto &str : strElements)
        {
            output.append(str);
            if (str != vec_back)
            {
                output.append(delimiter);
            }
        }
        return output;
    }

    std::vector<std::string> stringSplit(const std::string& str, const std::string& delimiter)
    {
        std::vector<std::string> split;

        size_t start = 0;
        size_t end = 0;
        bool running = true;

        while (running) {
            end = str.find(delimiter, start);
            if (end == std::string::npos) {
                end = str.size();
                running = false;
            }
            split.push_back(str.substr(start, end-start));
            start = end + delimiter.size();
        }

        return split;
    }
} // namespace ORCore
