#pragma once

#include <tsl/robin_map.h>

#include "dzstring.h"

template<typename T>
using Map = tsl::robin_map<const DzString*, T, DzString::Hash>;
