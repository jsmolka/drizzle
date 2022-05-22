#pragma

#include "utils.h"

namespace tests_vm_list {

inline suite _ = [] {
  "vm_list"_test = [] {
    {
      constexpr const char* kSources[] = {
R"(
var x = []
assert(x.size() == 0)
)",
R"(
var x = [1, 2, 3]
assert(x.size() == 3)
)",
R"(
var x = [1, 2, 3]
assert(x.size() == 3)
x.clear()
assert(x.size() == 0)
)",
R"(
var x = []
assert(x.push(1) == 1)
assert(x.push(2) == 2)
assert(x.push(3) == 3)
assert(x.push(4, 5, 6) == 6)
)",
      };

      for (const auto& source : kSources) {
        run(source);
      }
    }
    {
      //constexpr const char* kSources[] = {
      //};

      //for (const auto& source : kSources) {
      //  runThrows(source);
      //}
    }
  };
};

}  // namespace tests_vm_list
