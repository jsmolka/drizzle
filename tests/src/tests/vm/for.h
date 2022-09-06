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
var l = []
for c in "test":
  l.push(c)
assert(l == ["t", "e", "s", "t"])
)",
R"(
var l = [0, 1, 2]
var r = []
for v in l:
  r.push(v)
  r.push(l.pop())
assert(r == [0, 2, 1, 1])
)",
R"(
var l = [0, 1, 2, 3]
var r = []
for v in l:
  if v % 2 != 0:
    continue
  r.push(v)
assert(r == [0, 2])
)",
R"(
var l = [0, 1, 2, 3]
var i = 0
for v in l:
  if v < 2:
    l.push(4)
  i = i + 1
assert(l == [0, 1, 2, 3, 4, 4])
assert(i == 6)
)",
R"(
var i = forward([1, 2, 3])
for v in i:
  assert(v == 1)
  break
for v in i:
  assert(v == 2)
  break
for v in i:
  assert(v == 3)
  break
for v in i:
  assert(false)
)",
R"(
var i = reverse([1, 2, 3])
for v in i:
  assert(v == 3)
  break
for v in i:
  assert(v == 2)
  break
for v in i:
  assert(v == 1)
  break
for v in i:
  assert(false)
)",
R"(
var i = 0
for v in []:
  i = i + 1
assert(i == 0)
)",
R"(
var i = 0
for v in forward([1, 2, 3]):
  i = i + 1
assert(i == 3)
)",
R"(
var i = 0
for v in forward(forward([1, 2, 3])):
  i = i + 1
assert(i == 3)
)",
R"(
var l = [0, 1, 2]
var i = forward(l)
l.clear()
for x in i:
  assert(false)
assert(true)
)",
R"(
var l = [0, 1, 2]
for x in l:
  l.clear()
  assert(x == 0)
assert(true)
)",
R"(
for i in 0 .. 3:
  if false:
    break
assert(true)
)",
R"(
var i = 0
for j in 0 .. 5:
  assert(i == j)
  i = i + 1
assert(true)
)",
R"(
var i = 0
for j in 0 .. 5:
  if false:
    break
  assert(i == j)
  i = i + 1
assert(true)
)",
      };

      for (const auto& source : kSources) {
        run(source);
      }
    }
    {
      constexpr const char* kSources[] = {
R"(forward(1))",
R"(reverse(forward([])))",
      };

      for (const auto& source : kSources) {
        runThrows(source);
      }
    }
  };
};

}  // namespace tests_vm_for
