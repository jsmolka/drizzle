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
    collect();
    return constructNoCollect<T>(std::forward<Args>(args)...);
  }

  template<typename T, typename... Args>
  auto constructNoCollect(Args&&... args) -> T* {
    if constexpr (std::same_as<T, DzString>) {
      T stack(std::forward<Args>(args)...);
      const auto iter = pool.find(&stack);
      if (iter == pool.end()) {
        const auto heap = allocate<T>(std::move(stack));
        pool.insert(heap);
        return heap;
      } else {
        return *iter;
      }
    } else {
      return allocate<T>(std::forward<Args>(args)...);
    }
  }

  void mark(const DzValue& value);
  void mark(DzObject* object);

  Vm* vm = nullptr;

private:
  static constexpr auto kGrowthFactor = 2;

  template<typename T, typename... Args>
  auto allocate(Args&&... args) -> T* {
    auto object = new T(std::forward<Args>(args)...);
    object->next = objects;
    objects = object;

    allocated += sizeof(T);
    if constexpr (std::same_as<T, DzString>) {
      allocated += object->data.size();
    }
    return object;
  }

  void collect();
  void mark();
  void sweep();

  DzObject* objects = nullptr;
  std::size_t allocated = 0;
  std::size_t threshold = 1024 * 1024;
  tsl::robin_set<DzString*, DzString::Hash, DzString::EqualData> pool;
};
