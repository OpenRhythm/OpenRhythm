// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "keycode.hpp"
#include <vector>

namespace ORCore
{
    std::vector<ModFlag> processModifiers(int value)
    {
        std::vector<ModFlag> keys;
        for (auto const& key : modMap)
        {
            if (value & key.first)
            {
                keys.push_back(key.second);
            }
        }
        return keys;
    }
}
