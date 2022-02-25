#include "utils.h"

namespace tests_vm_function {

inline suite _ = [] {
  "vm_function"_test = [] {
    {
      constexpr const char* kSources[] = {
R"(
def add(a, b):
  return a + b
assert(add(1, 2) == 3)
assert(add(add(1, 2), add(3, 4)) == 10)
)",
R"(
def fib(n):
  if n < 2:
    return n
  return fib(n - 2) + fib(n - 1)
assert(fib(4) == 3)
)",
R"(
def outer(n):
  def inner(n):
    assert(n == 1)
  inner(n)
outer(1)
)",
R"(
def outer():
  def inner(n):
    assert(n == 1)
  return inner
outer()(1)
)",
      };

      for (const auto& source : kSources) {
        run(source);
      }
    }
    {
      constexpr const char* kSources[] = {
        "1()",
        "null()",
        "assert(1, 2)",
      };

      for (const auto& source : kSources) {
        runThrows(source);
      }
    }
  };
};

}  // namespace tests_vm_function
