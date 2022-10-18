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

  virtual operator bool() const override;
  virtual auto operator==(const DzObject& other) const -> bool override;
  virtual auto size() const -> std::size_t override;
  virtual auto repr() const -> std::string override;

  auto get(const DzValue& key) const -> std::optional<DzValue>;
  void set(const DzValue& key, const DzValue& value);

  virtual auto getExpr(Vm& vm, const DzValue& expr) -> DzValue override;
  virtual void setExpr(Vm& vm, const DzValue& expr, const DzValue& value) override;

  tsl::robin_map<DzValue, DzValue, std::hash<DzValue>, Equal> values;
};
