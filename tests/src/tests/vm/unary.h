#pragma

#include "utils.h"

namespace tests_vm_unary {

inline suite _ = [] {
  static_assert(int(Expression::Unary::Type::LastEnumValue) == 3);

  "vm_unary"_test = [] {
    {
      constexpr const char* kSources[] = {
R"(
assert(~1 == -2)
assert(~2 == -3)
)",
R"(
assert(-1 == -1)
assert(-2 == -2)
)",
R"(
assert(!false)
assert(!0)
assert(!null)
assert(!"")
assert(!0.0)
)",
      };

      for (const auto& source : kSources) {
        run(source);
      }
    }
    {
      constexpr const char* kSources[] = {
        "-null",
        "~null",
        "~0.0",
      };

      for (const auto& source : kSources) {
        runThrows(source);
      }
    }
  };
};

}  // namespace tests_vm_unary
