#pragma once

#include <optional>
#include <tsl/robin_map.h>

#include "dzobject.h"

class DzMap final : public DzObject {
public:
  struct Equal {
    auto operator()(const DzValue& a, const DzValue& b) const -> bool;
  };

  DzMap();

  explicit operator bool() const override final;
  auto operator==(const DzObject& other) const -> bool override final;
  auto size() const -> std::size_t override final;
  auto repr() const -> std::string override final;

  auto in(Vm& vm, const DzValue& value) -> bool override final;
  auto getExpr(Vm& vm, const DzValue& expr) -> DzValue override final;
  auto getProp(Vm& vm, const DzValue& prop, bool bind) -> DzValue override final;
  void setExpr(Vm& vm, const DzValue& expr, const DzValue& value) override final;

  auto get(const DzValue& key) const -> std::optional<DzValue>;
  void set(const DzValue& key, const DzValue& value);

  tsl::robin_map<DzValue, DzValue, std::hash<DzValue>, Equal> values;

private:
  void members(Vm& vm);
};
