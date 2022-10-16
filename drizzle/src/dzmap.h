#pragma once

#include <optional>
#include <tsl/robin_map.h>

#include "dzobject.h"

class DzMap : public DzObject {
public:
  struct Equal {
    auto operator()(const DzValue& a, const DzValue& b) const -> bool {
      return DzValue::binary(a, b, []<typename A, typename B>(const A& a, const B& b) {
        if constexpr (dz_primitive<A, B> || dz_object<A, B>) {
          return a == b;
        }
        return false;
      });
    }
  };

  DzMap();

  operator bool() const;
  auto operator==(const DzMap& other) const -> bool;
  auto repr() const -> std::string;

  auto get(const DzValue& key) const -> std::optional<DzValue>;
  void set(const DzValue& key, const DzValue& value);

  auto subscriptGet(Vm& vm, const DzValue& expr) -> DzValue;
  void subscriptSet(Vm& vm, const DzValue& expr, const DzValue& value);

  tsl::robin_map<DzValue, DzValue, std::hash<DzValue>, Equal> values;
};
