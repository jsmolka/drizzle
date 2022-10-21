#pragma once

#include "dzobject.h"

class DzIterator : public DzObject {
public:
  DzIterator(DzObject* iteree);

  explicit operator bool() const override final;
  void mark(Gc& gc) override final;

  auto makeIterator(Vm&) -> DzValue override final;

  virtual auto done() const -> bool = 0;
  virtual void advance() = 0;
  virtual auto value(Vm&) const -> DzValue = 0;

  mutable DzObject* iteree;
};

class DzSequenceIterator final : public DzIterator {
public:
  DzSequenceIterator(DzObject* iteree);

  auto done() const -> bool override final;
  void advance() override final;
  auto value(Vm& vm) const -> DzValue override final;

private:
  std::size_t index;
};

class DzSequenceReverseIterator final : public DzIterator {
public:
  DzSequenceReverseIterator(DzObject* iteree);

  auto done() const -> bool override final;
  void advance() override final;
  auto value(Vm& vm) const -> DzValue override final;

private:
  std::size_t index;
};

