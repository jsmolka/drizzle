#pragma once

#include <tsl/robin_set.h>

#include "dzstring.h"
#include "dzvalue.h"

class Vm;

class Gc {
public:
  ~Gc();

  template<typename T, typename... Args>
  auto construct(Args&&... args) -> T* {
    auto allocate = [this]<typename... LambdaArgs>(LambdaArgs&&... args) {
      auto object = new T(std::forward<LambdaArgs>(args)...);
      object->next = objects;
      objects = object;

      allocated += sizeof(T);
      if constexpr (std::same_as<T, DzString>) {
        allocated += object->data.size();
      }
      return object;
    };

    collect();
    if constexpr (std::same_as<T, DzString>) {
      DzString string(std::forward<Args>(args)...);
      const auto iter = pool.find(&string);
      return iter == pool.end() 
        ? *pool.insert(allocate(std::move(string))).first
        : *iter;
    } else {
      return allocate(std::forward<Args>(args)...);
    }
  }

  Vm* vm = nullptr;

private:
  static constexpr auto kGrowthFactor = 2;

  void collect();
  void mark();
  void mark(const DzValue& value);
  void mark(DzObject* object);
  void sweep();

  DzObject* objects = nullptr;
  std::size_t allocated = 0;
  std::size_t threshold = 1024 * 1024;
  tsl::robin_set<DzString*, DzString::Hash, DzString::Equal> pool;
};
