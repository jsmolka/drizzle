#pragma once

#include <optional>
#include <tsl/robin_map.h>

#include "dzstring.h"

template<typename Key, typename T, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
class Map : public tsl::robin_map<Key, T, Hash, KeyEqual> {
public:
  auto get(const Key& key) const -> std::optional<T> {
    const auto iter = this->find(key);
    return iter != this->end()
      ? iter->second
      : std::optional<T>();
  }

  void set(const Key& key, const T& value) {
    this->insert_or_assign(key, value);
  }
};

template<typename T>
using StringMap = Map<const DzString*, T, DzString::Hash, DzString::Equal>;
