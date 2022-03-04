#pragma once

#include <concepts>

#include "dzobject.h"

class Vm;

class Gc {
public:
  ~Gc();

  template<typename T, typename... Args>
    requires std::is_base_of_v<DzObject, T> && std::constructible_from<T, Args...>
  T* allocate(Args&&... args) {
    auto object = new T(std::forward<Args>(args)...);
    object->next = objects;
    objects = object;
    return object;
  }

  Vm* vm = nullptr;

private:
  DzObject* objects = nullptr;
};
