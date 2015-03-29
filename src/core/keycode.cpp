#include <vector>
#include "keycode.hpp"

namespace MgCore
{
    std::vector<ModFlag> processModifiers(int value)
    {
        std::vector<ModFlag> keys;
        for (auto const& key : modMap) {
            if (value & key.first) {
                keys.push_back(key.second);
            }
        }
        return keys;
    }
}