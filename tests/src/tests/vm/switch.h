#pragma

#include "utils.h"

namespace tests_vm_switch {

inline suite _ = [] {
  "vm_switch"_test = [] {
    constexpr const char* kSources[] = {
R"(
switch 0:
  case 0:
    assert(true)
  case 1:
    assert(false)
)",
R"(
switch 1:
  case 0:
    assert(false)
  case 1:
    assert(true)
)",
R"(
var x
switch 1:
  case 0:
    x = 0
  case 1:
    x = 1
assert(x == 1)
)",
R"(
class Value:
  def init(value):
    this.count = 0
    this.value = value
  def get():
    this.count = this.count + 1
    return this.value
    
var v = Value(0)

switch v.get():
  case 0:
    assert(true)
  default:
    assert(false)

assert(v.count == 1)
)",
    };

    for (const auto& source : kSources) {
      run(source);
    }
  };
};

}  // namespace tests_vm_switch
