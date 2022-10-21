#pragma once

#include "dzobject.h"

class DzString final : public DzObject {
public:
  struct Hash {
    auto operator()(const DzString* string) const -> std::size_t;
  };

  struct Equal {
    auto operator()(const DzString* a, const DzString* b) const -> bool;
  };

  struct EqualData {
    auto operator()(const DzString* a, const DzString* b) const -> bool;
  };

  DzString();
  DzString(const char* data);
  DzString(std::string_view data);
  DzString(const std::string& data);

  explicit operator bool() const override final;
  auto hash() const -> std::size_t override final;
  auto size() const -> std::size_t override final;
  auto repr() const -> std::string override final;

  auto makeIterator(Vm& vm) -> DzValue override final;
  auto makeReverseIterator(Vm& vm) -> DzValue override final;
  auto in(Vm& vm, const DzValue& value) -> bool override final;
  auto getItem(Vm& vm, std::size_t index) -> DzValue override final;
  auto getExpr(Vm& vm, const DzValue& expr) -> DzValue override final;

  std::string data;
  std::size_t data_hash;

private:
  auto toIndex(Vm& vm, const DzValue& expr) const -> std::size_t;
};
