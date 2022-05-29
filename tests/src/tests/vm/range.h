#pragma

#include "utils.h"

namespace tests_vm_range {

inline suite _ = [] {
  "vm_range"_test = [] {
    {
      constexpr const char* kSources[] = {
R"(
var l = []
for x in range(0, 10, 1):
  l.push(x)
assert(l == [0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
)",
R"(
var l = []
for x in range(0, 10, 2):
  l.push(x)
assert(l == [0, 2, 4, 6, 8])
)",
R"(
var l = []
for x in 0 .. 10:
  l.push(x)
assert(l == [0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
)",
R"(
var l = []
for x in reverse(range(0, 10, 2)):
  l.push(x)
assert(l == [8, 6, 4, 2, 0])
)",
R"(
var l = []
for x in range(10, 0, -1):
  l.push(x)
assert(l == [10, 9, 8, 7, 6, 5, 4, 3, 2, 1])
)",
R"(
var l = []
for x in range(10, 0, -2):
  l.push(x)
assert(l == [10, 8, 6, 4, 2])
)",
R"(
var l = []
for x in reverse(range(10, 0, -2)):
  l.push(x)
assert(l == [2, 4, 6, 8, 10])
)",
      };

      for (const auto& source : kSources) {
        run(source);
      }
    }
    {
      constexpr const char* kSources[] = {
R"(range(0, 0, 0))",
      };

      for (const auto& source : kSources) {
        runThrows(source);
      }
    }
  };
};

}  // namespace tests_vm_range
