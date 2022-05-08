#pragma once

#include <tsl/robin_map.h>

#include "dzstring.h"

template<typename T>
using Map = tsl::robin_map<DzString*, T, DzString::Hash, DzString::Equal>;
