#pragma

#include "utils.h"

namespace tests_vm_string {

inline suite _ = [] {
  "vm_string"_test = [] {
    {
      constexpr const char* kSources[] = {
R"(
assert(len(" ") == 1)
assert(len("  ") == 2)
assert(len("   ") == 3)
)",
R"(
assert("abc"[0] == "a")
assert("abc"[1] == "b")
assert("abc"[2] == "c")
assert("abc"[-1] == "c")
assert("abc"[-2] == "b")
assert("abc"[-3] == "a")
)",
R"(
assert("test" in "_test_")
assert(!("test" in "_est_"))
)",
      };

      for (const auto& source : kSources) {
        run(source);
      }
    }
    {
      constexpr const char* kSources[] = {
        R"(1 in "")",
      };

      for (const auto& source : kSources) {
        runThrows(source);
      }
    }
  };
};

}  // namespace tests_vm_string
