#include "stringpool.h"

StringPool::~StringPool()
{
    for (auto& [hash, string] : pool)
        delete string;
}

DzString* StringPool::make(std::string data)
{
    auto hash = robin_hood::hash<std::string>()(data);
    auto iter = pool.find(hash);
    if (iter != pool.end())
    {
        return iter->second;
    }
    else
    {
        auto string = new DzString(std::move(data), hash);
        pool.insert({ hash, string });
        return string;
    }
}
