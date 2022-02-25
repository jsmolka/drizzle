#pragma

#include "utils.h"

namespace tests_vm_branch {

inline suite _ = [] {
  "vm_branch"_test = [] {
    constexpr const char* kSources[] = {
R"(
var x
if true:
  x = 0
else:
  x = 1
assert(x == 0)
)",
R"(
var x
if false:
  x = 0
else:
  x = 1
assert(x == 1)
)",
R"(
var x
if false:
  noop
elif false:
  noop
elif true:
  x = 1
assert(x == 1)
)",
    };

    for (const auto& source : kSources) {
      run(source);
    }
  };
};

}  // namespace tests_vm_branch
