#pragma

#include "utils.h"

namespace tests_vm_scope {

inline suite _ = [] {
  "vm_scope"_test = [] {
    constexpr const char* kSources[] = {
R"(
var x = 0
block:
  var x = 1
  assert(x == 1)
assert(x == 0)
)",
R"(
var x = 0
block:
  var x = x
  assert(x == 0)
assert(x == 0)
)",
R"(
var x = 0
block:
  var x = x
  x = 1
  assert(x == 1)
assert(x == 0)
)",
    };

    for (const auto& source : kSources) {
      run(source);
    }
  };
};

}  // namespace tests_vm_scope
