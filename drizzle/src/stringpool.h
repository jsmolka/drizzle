#pragma once

#include "dzmap.h"
#include "dzstring.h"

class StringPool
{
public:
    ~StringPool();

    DzString* make(std::string data);

private:
    DzMap<DzString*> pool;
};
