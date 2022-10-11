#pragma once

#include "dziterator.h"

class DzString : public DzObject {
public:
  struct Hash {
    auto operator()(const DzString* string) const -> std::size_t {
      return string->hash;
    }
  };

  struct Equal {
    auto operator()(const DzString* a, const DzString* b) const -> bool {
      return a->data == b->data;
    }
  };

  DzString();
  DzString(const char* data);
  DzString(std::string_view data);
  DzString(const std::string& data);

  operator bool() const;
  auto operator[](std::size_t index) const -> std::string_view;
  auto repr() const -> std::string;
  auto size() const -> std::size_t;

  virtual auto subscriptGet(Vm& vm, const DzValue& expr) const -> std::optional<DzValue> override;

  std::string data;
  std::size_t hash;
};

class DzStringIterator : public DzIterator {
public:
  DzStringIterator(DzObject* iteree);

  virtual auto done() const -> bool final;
  virtual void advance() final;
  virtual auto current(Gc& gc) const -> DzValue final;

private:
  std::size_t index;
};

class DzStringReverseIterator : public DzIterator {
public:
  DzStringReverseIterator(DzObject* iteree);

  virtual auto done() const -> bool final;
  virtual void advance() final;
  virtual auto current(Gc& gc) const -> DzValue final;

private:
  std::size_t index;
};
