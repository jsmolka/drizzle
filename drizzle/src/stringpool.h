#pragma once

#include "dzstring.h"
#include "dztable.h"

class StringPool
{
public:
    ~StringPool();

    DzString* make(std::string&& data);
    DzString* make(const std::string& data);

private:
    DzTable<DzString*> pool;
};
