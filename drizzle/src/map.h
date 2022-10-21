#pragma once

#include <optional>
#include <tsl/robin_map.h>

#include "dzstring.h"

template<typename T>
class Map : public tsl::robin_map<const DzString*, T, DzString::Hash, DzString::Equal> {
public:
  auto get(const DzString* identifier) const -> std::optional<T> {
    const auto iter = this->find(identifier);
    return iter != this->end()
      ? iter->second
      : std::optional<T>();
  }

  void set(const DzString* identifier, const T& value) {
    this->insert_or_assign(identifier, value);
  }
};
