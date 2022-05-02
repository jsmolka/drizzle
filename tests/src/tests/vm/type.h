#pragma

#include "utils.h"

namespace tests_vm_type {

inline suite _ = [] {
  "vm_type"_test = [] {
    constexpr auto kSource = R"(
class Test:
  def method():
    noop
def test():
  noop
assert(type(1) == "int")
assert(type(1.1) == "float")
assert(type("test") == "string")
assert(type(null) == "null")
assert(type(true) == "bool")
assert(type(false) == "bool")
assert(type(Test) == "class")
assert(type(Test().method) == "function")
assert(type(test) == "function")
)";
    run(kSource);
  };
};

}  // namespace tests_vm_type
