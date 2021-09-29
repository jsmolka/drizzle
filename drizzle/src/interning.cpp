#include "interning.h"

Interning::~Interning()
{
    for (auto& [hash, string] : interned)
    {
        delete string;
    }
}

DzString* Interning::make(std::string&& data)
{
    auto hash = robin_hood::hash<std::string>()(data);
    auto iter = interned.find(hash);
    if (iter != interned.end())
    {
        return iter->second;
    }
    else
    {
        auto string = new DzString(std::move(data), hash);
        interned.insert({ hash, string });
        return string;
    }
}

DzString* Interning::make(const std::string& data)
{
    return make(std::string(data));
}
