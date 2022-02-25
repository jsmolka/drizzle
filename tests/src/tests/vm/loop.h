#pragma

#include "utils.h"

namespace tests_vm_loop {

inline suite _ = [] {
  "vm_loop"_test = [] {
    constexpr const char* kSources[] = {
R"(
var x = 0
while x < 10:
  x = x + 1
assert(x == 10)
)",
R"(
var x = 0
while true:
  x = x + 1
  if x == 10:
    break
assert(x == 10)
)",
R"(
var x = 0
var y = 0
while x < 10:
  x = x + 1
  if x % 2 == 0:
    continue
  y = y + 1
assert(y == 5)
)",
    };

    for (const auto& source : kSources) {
      run(source);
    }
  };
};

}  // namespace tests_vm_loop
