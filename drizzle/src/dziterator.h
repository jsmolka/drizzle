#pragma once

#include "dzobject.h"
#include "dzvalue.h"

class DzIterator : public DzObject {
public:
  DzIterator(DzObject* iteree);

  virtual explicit operator bool() const override;

  virtual auto done() const -> bool = 0;
  virtual void advance() = 0;
  virtual auto value(Vm&) const -> DzValue = 0;

  virtual auto makeIterator(Vm&) -> DzValue override;

  mutable DzObject* iteree;

private:
  std::size_t index;
};

class DzSequenceIterator : public DzIterator {
public:
  DzSequenceIterator(DzObject* iteree);

  auto done() const -> bool override;
  void advance() override;
  auto value(Vm& vm) const -> DzValue override;

private:
  std::size_t index;
};

class DzSequenceReverseIterator : public DzIterator {
public:
  DzSequenceReverseIterator(DzObject* iteree);

  auto done() const -> bool override;
  void advance() override;
  auto value(Vm& vm) const -> DzValue override;

private:
  std::size_t index;
};

