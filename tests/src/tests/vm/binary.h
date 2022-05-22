#pragma

#include "utils.h"

namespace tests_vm_binary {

inline suite _ = [] {
  static_assert(int(Expression::Binary::Type::LastEnumValue) == 21);

  "vm_binary"_test = [] {
    {
      constexpr const char* kSources[] = {
R"(
assert(true && true)
assert(true && true && true)
assert(!(false && true))
assert(!(false && assert(false)))
)",
R"(
assert(true || true)
assert(true || false)
assert(true || assert(false))
)",
R"(
assert(1 + 1 == 2)
assert(1 + 1.0 == 2.0)
assert(true + true == 2)
assert(true + 0 == 1)
)",
R"(
assert((true & true) == true)
assert((1 & 3) == 1)
assert((true & 3) == 1)
assert((0xFF & 0xF) == 0xF)
)",
R"(
assert((2 >> 1) == 1)
assert((2 >> true) == true)
assert((0x8000000000000000 >> 3) == 0xF000000000000000)
)",
R"(
assert((1 << 2) == 4)
assert((true << 2) == 4)
)",
R"(
assert((2 >>> 1) == 1)
assert((2 >>> true) == true)
assert((0x8000000000000000 >>> 3) == 0x1000000000000000)
)",
R"(
assert((true | false) == true)
assert((1 | 2 | 4) == 7)
assert((0xFF | 0xFF00) == 0xFFFF)
)",
R"(
assert((1 ^ 0) == 1)
assert((1 ^ 1) == 0)
)",
R"(
assert(1 / 2 == 0.5)
assert(2 / 2 == 1)
assert(true / 2 == 0.5)
)",
R"(
assert(1 > 0)
assert(true > false)
assert(1.5 > 1)
)",
R"(
assert(1 >= 0)
assert(1 >= 1)
assert(true >= 0)
assert(1.5 >= 1)
)",
R"(
assert(5 // 2 == 2)
assert(5.5 // 2 == 2.0)
)",
R"(
assert(0 < 1)
assert(false < true)
assert(1 < 1.5)
)",
R"(
assert(0 <= 0)
assert(false <= true)
assert(1 <= 1.5)
)",
R"(
assert(3 % 2 == 1)
assert(3 % -2 == 1)
assert(3 % 2.5 == 0.5)
assert(3 % -2.5 == 0.5)
)",
R"(
assert(1 * 1 == 1)
assert(2 * 2.5 == 5)
assert(true * false == 0)
)",
R"(
assert(1 == 1)
assert(true == true)
assert(1.5 == 1.5)
assert(null == null)
assert("a" == "a")
assert(!(0 == null))
)",
R"(
assert(1 != 2)
assert(true != false)
assert(1.5 != 2)
assert("a" != null)
assert("a" != "b")
assert(0 != null)
)",
R"(
assert(10**2 == 100)
assert(2**3 == 8)
)",
R"(
assert(1 - 1 == 0)
assert(1.5 - 1 == 0.5)
assert(false - true == -1)
)",
      };

      for (const auto& source : kSources) {
        run(source);
      }
    }
    {
      constexpr const char* kSources[] = {
        "1.0 & 1",
        "null & 1",
        "1.0 >> 1",
        "null >> 1",
        "1.0 << 1",
        "null << 1",
        "1.0 >>> 1",
        "null >>> 1",
        "1.0 | 1",
        "null | 1",
        "1.0 ^ 1",
        "null ^ 1",
        "1 / 0",
        "null / 1",
        "1 // 0",
        "null // 1",
        "1 % 0",
        "null % 1",
      };

      for (const auto& source : kSources) {
        runThrows(source);
      }
    }
  };
};

}  // namespace tests_vm_binary
