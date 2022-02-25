#include "utils.h"

namespace tests_vm_branch {

inline suite _ = [] {
  "vm_branch"_test = [] {
    constexpr const char* kSources[] = {
R"(
if true:
  assert(true)
else:
  assert(false)
)",
R"(
if false:
  assert(false)
else:
  assert(true)
)",
R"(
var x = 2
var y = 0
if x < 1:
  y = 1
elif x < 2:
  y = 2
elif x < 3:
  y = 3
assert(y == 3)
)",
    };

    for (const auto& source : kSources) {
      run(source);
    }
  };
};

}  // namespace tests_vm_branch
