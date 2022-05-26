#pragma

#include "utils.h"

namespace tests_vm_for {

inline suite _ = [] {
  "vm_for"_test = [] {
    {
      constexpr const char* kSources[] = {
R"(
var i = 0
for v in [0, 1, 2]:
  assert(i == v)
  i = i + 1
assert(i == 3)
)",
R"(
var l = [0, 1, 2]
var ll = []
for v in l:
  ll.push(v)
  ll.push(l.pop())
assert(ll == [0, 2, 1, 1])
)",
R"(
var l = []
for c in "test":
  l.push(c)
assert(l == ["t", "e", "s", "t"])
)",
      };

      for (const auto& source : kSources) {
        run(source);
      }
    }
  };
};

}  // namespace tests_vm_for
