#pragma

#include "utils.h"

namespace tests_vm_assert {

inline suite _ = [] {
  "vm_assert"_test = [] {
    {
      constexpr auto kSource = "assert(true)";
      run(kSource);
    }
    {
      constexpr auto kSource = "assert(false)";
      runThrows(kSource);
    }
  };
};

}  // namespace tests_vm_assert
