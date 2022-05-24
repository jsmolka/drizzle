#pragma

#include "utils.h"

namespace tests_vm_string {

inline suite _ = [] {
  "vm_string"_test = [] {
    {
      constexpr const char* kSources[] = {
R"(
assert(" ".size() == 1)
assert("  ".size() == 2)
assert("   ".size() == 3)
)",
      };

      for (const auto& source : kSources) {
        run(source);
      }
    }
    {
      constexpr const char* kSources[] = {
        R"("".size(1))",
      };

      for (const auto& source : kSources) {
        runThrows(source);
      }
    }
  };
};

}  // namespace tests_vm_string
