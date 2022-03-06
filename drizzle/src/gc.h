#pragma once

#include "dzvalue.h"

class Vm;

class Gc {
public:
  ~Gc();

  template<typename T, typename... Args>
    requires std::is_base_of_v<DzObject, T> && std::constructible_from<T, Args...>
  auto allocate(Args&&... args) -> T* {
    collect();

    auto object = new T(std::forward<Args>(args)...);
    object->next = objects;
    objects = object;
    allocated += sizeof(T);

    return object;
  }

  Vm* vm = nullptr;

private:
  static constexpr auto kGrowthFactor = 2;

  void collect();
  void mark();
  void mark(DzValue& value);
  void mark(DzObject* object);
  void sweep();

  DzObject* objects = nullptr;
  std::size_t allocated = 0;
  std::size_t threshold = 1024 * 1024;
};
