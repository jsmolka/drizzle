#pragma once

#include "dziterator.h"

class DzRange final : public DzObject {
public:
  DzRange(dzint start, dzint stop, dzint step);

  auto operator==(const DzObject& other) const -> bool override final;
  auto repr() const -> std::string override final;

  auto makeIterator(Vm& vm) -> DzValue override final;
  auto makeReverseIterator(Vm& vm) -> DzValue override final;
  auto in(Vm& vm, const DzValue& value) -> bool override final;

private:
  dzint start;
  dzint stop;
  dzint step;
};

class DzRangeIterator final : public DzIterator {
public:
  DzRangeIterator(DzRange* iteree, dzint iter, dzint stop, dzint step);

  auto done() const -> bool final;
  void advance() final;
  auto value(Vm&) const -> DzValue final;

private:
  dzint iter;
  dzint stop;
  dzint step;
};
