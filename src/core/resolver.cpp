// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "resolver.hpp"
namespace ORCore
{
    std::unordered_map<std::type_index, std::unique_ptr<Resolver::Placeholder>> Resolver::m_services;
}