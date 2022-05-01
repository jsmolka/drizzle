#pragma

#include "utils.h"

namespace tests_vm_capture {

inline suite _ = [] {
  "vm_capture"_test = [] {
    constexpr auto kSource = R"(
var x = 0
def set_x(value):
  x = value
def get_x():
  return x
assert(get_x() == 0)
set_x(1)
assert(get_x() == 1)
set_x(2)
assert(get_x() == 2)
)";
    run(kSource);
  };
};

}  // namespace tests_vm_capture
