#pragma once

#include <sh/vector.h>

#include "dziterator.h"

class DzBytes : public DzObject {
public:
  DzBytes();

  operator bool() const;
  auto operator==(const DzBytes& other) const -> bool;
  auto operator[](std::size_t index) -> u8&;
  auto operator[](std::size_t index) const -> const u8&;
  auto repr() const -> std::string;
  auto size() const -> std::size_t;

  virtual auto subscriptGet(Vm& vm, const DzValue& expr) -> DzValue override;
  virtual void subscriptSet(Vm& vm, const DzValue& expr, const DzValue& value) override;

  sh::vector<u8> data;
};

class DzBytesIterator : public DzIterator {
public:
  DzBytesIterator(DzObject* iteree);

  virtual auto done() const -> bool final;
  virtual void advance() final;
  virtual auto current(Gc&) const -> DzValue final;

private:
  std::size_t index;
};

class DzBytesReverseIterator : public DzIterator {
public:
  DzBytesReverseIterator(DzObject* iteree);

  virtual auto done() const -> bool final;
  virtual void advance() final;
  virtual auto current(Gc&) const -> DzValue final;

private:
  std::size_t index;
};
