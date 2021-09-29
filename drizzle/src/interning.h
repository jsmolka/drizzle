#pragma once

#include "dzstring.h"
#include "dztable.h"

class Interning
{
public:
    ~Interning();

    DzString* make(std::string&& data);
    DzString* make(const std::string& data);

private:
    DzTable<DzString*> interned;
};
