#pragma once

#include "dzobject.h"
#include "dzvalue.h"

class Gc;

class DzIterator : public DzObject {
public:
  DzIterator(std::string_view name, DzObject* iteree);

  operator bool() const;
  auto repr() const -> std::string;

  virtual void advance() = 0;
  virtual auto dereference(Gc&) const -> DzValue = 0;

  DzObject* iteree;

protected:
  template<typename T>
    requires std::is_base_of_v<DzObject, T>
  auto as() const -> const T& {
    assert(iteree);
    return *static_cast<const T*>(iteree);
  }

private:
  std::string_view name;
};
