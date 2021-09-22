#pragma once

#include <shell/functional.h>
#include <robin_hood.h>

#include "dzstring.h"

class Interning
{
public:
    ~Interning();

    DzString* make(std::string&& data);

private:
    robin_hood::unordered_flat_map<std::size_t, DzString*, shell::Identity> interned;
};
