#pragma

#include "utils.h"

namespace tests_vm_globals {

inline suite _ = [] {
  "vm_globals"_test = [] {
    {
      constexpr const char* kSources[] = {
R"(
def a():
  assert(true)
  b()
def b():
  assert(true)
a()
)",
R"(
def a():
  assert(true)
  b()
def c():
  assert(true)
var b = c
a()
)",
R"(
var x = 0
def set(value):
  x = value
def get():
  return x
assert(get() == 0)
set(1)
assert(get() == 1)
set(2)
assert(get() == 2)
)",
      };

      for (const auto& source : kSources) {
        run(source);
      }
    }
    {
      constexpr const char* kSources[] = {
R"(
def a():
  assert(true)
  b()
a()
def b():
  assert(true)
)",
R"(
def a():
  assert(true)
  b()
var b = c
def c():
  assert(true)
a()
)",
      };

      for (const auto& source : kSources) {
        runThrows(source);
      }
    }
  };
};

}  // namespace tests_vm_globals
