#pragma

#include "utils.h"

namespace tests_vm_repr {

inline suite _ = [] {
  "vm_repr"_test = [] {
    constexpr auto kSource = R"(
class Test:
  def method():
    noop
def test():
  noop
assert(repr(1) == "1")
assert(repr(1.1) == "1.1")
assert(repr("test") == "test")
assert(repr(null) == "null")
assert(repr(true) == "true")
assert(repr(false) == "false")
assert(repr(Test) == "<class Test>")
assert(repr(Test().method) == "<function method>")
assert(repr(test) == "<function test>")
)";
    run(kSource);
  };
};

}  // namespace tests_vm_repr
