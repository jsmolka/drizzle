#pragma once

#include <robin_hood/robin_hood.h>
#include <shell/functional.h>

template<typename T>
using DzMap = robin_hood::unordered_flat_map<std::size_t, T, shell::Identity>;
