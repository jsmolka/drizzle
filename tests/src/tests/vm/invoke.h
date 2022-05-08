#pragma

#include "utils.h"

namespace tests_vm_invoke {

inline suite _ = [] {
  "vm_invoke"_test = [] {
    constexpr const char* kSources[] = {
R"(
class Outer:
  def inner():
    class Inner:
      def add(x, y):
        return x + y
    return Inner
var o = Outer()
var i = o.inner()()
assert(i.add(1, 2) == 3)
)",
R"(
class Test:
  def invoke():
    return 1
var t = Test()
assert(t.invoke() == 1)
)",
R"(
class Test:
  def invoke():
    return 1
assert(Test().invoke() == 1)
)",
    };

    for (const auto& source : kSources) {
      run(source);
    }
  };
};

}  // namespace tests_vm_invoke
