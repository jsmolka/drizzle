#pragma once

#include "dzvalue.h"

class Vm;

class Gc {
public:
  ~Gc();

  template<typename T, typename... Args>
    requires std::is_base_of_v<DzObject, T> && std::constructible_from<T, Args...>
  T* allocate(Args&&... args) {
    if (vm) {
      collect();
    }

    auto object = new T(std::forward<Args>(args)...);
    object->next = objects;
    objects = object;
    fmt::print("alloc {}\n", object->repr());
    return object;
  }

  Vm* vm = nullptr;

private:
  void collect();
  void mark();
  void mark(DzValue& value);
  void mark(DzObject* object);
  void sweep();

  DzObject* objects = nullptr;
};
