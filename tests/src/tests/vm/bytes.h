#pragma

#include "utils.h"

namespace tests_vm_bytes {

inline suite _ = [] {
  "vm_bytes"_test = [] {
    {
      constexpr const char* kSources[] = {
R"(
var x = bytes()
assert(x.size() == 0)
)",
R"(
var x = bytes(1, 2, 3)
assert(x.size() == 3)
assert(x[0] == 1)
assert(x[1] == 2)
assert(x[2] == 3)
)",
R"(
var x = bytes(1, 2, 3)
assert(x.size() == 3)
x.clear()
assert(x.size() == 0)
)",
R"(
var x = bytes()
assert(x.push(1) == 1)
assert(x.push(2) == 2)
assert(x.push(3) == 3)
assert(x.push(4, 5, 6) == 6)
assert(x[0] == 1)
assert(x[5] == 6)
)",
R"(
var x = bytes()
assert(x.insert(0, 1) == 1)
assert(x.size() == 1)
assert(x[0] == 1)
assert(x.insert(0, 2) == 2)
assert(x.size() == 2)
assert(x[0] == 2)
assert(x.insert(1, 3) == 3)
assert(x.size() == 3)
assert(x[1] == 3)
)",
R"(
var x = bytes(1, 2, 3)
assert(x.pop() == 3)
assert(x.pop() == 2)
assert(x.pop() == 1)
assert(x.size() == 0)
)",
R"(
var x = bytes(1, 2, 3)
assert(x.remove(1) == 2)
assert(x.remove(0) == 1)
assert(x.remove(0) == 3)
assert(x.size() == 0)
)",
R"(
var x = bytes(1, 2, 3, 1)
assert(x.index(1) == 0)
assert(x.index(2) == 1)
assert(x.index(3) == 2)
assert(x.index(4) == -1)
)",
R"(
var x = bytes()
x.resize(8, 0)
assert(x.size() == 8)
assert(x[0] == 0)
assert(x[7] == 0)
x.resize(4, 0)
assert(x.size() == 4)
assert(x[0] == 0)
assert(x[3] == 0)
x.resize(8, 1)
assert(x.size() == 8)
assert(x[0] == 0)
assert(x[7] == 1)
)",
R"(
var x = bytes(1, 2, 3)
assert(x.insert(-3, 0) == 4)
assert(x[0] == 0)
)",
R"(
var x = bytes(1, 2, 3)
assert(x[-1] == 3)
assert(x[-2] == 2)
assert(x[-3] == 1)
)",
R"(
var x = bytes(1, 2, 3)
assert(x.remove(-2) == 2)
assert(x.remove(-1) == 3)
assert(x.remove(-1) == 1)
)",
R"(
var x = bytes() + bytes()
assert(x.size() == 0)
)",
R"(
var x = bytes(1) + bytes(2, 3)
assert(x.size() == 3)
assert(x[0] == 1)
assert(x[1] == 2)
assert(x[2] == 3)
)",
R"(
assert(bytes(1, 2, 3) == bytes(1, 2, 3))
assert(bytes(1, 2, 2) != bytes(1, 2, 3))
)",
R"(
assert(1 in bytes(1))
assert(!(1 in bytes(2)))
)",
      };

      for (const auto& source : kSources) {
        run(source);
      }
    }
    {
      constexpr const char* kSources[] = {
        R"(bytes().push("test"))",
        R"(bytes().push(1, "test"))",
        R"(bytes().insert())",
        R"(bytes().insert(1))",
        R"(bytes().insert("test", 0))",
        R"(bytes().insert(0, "test"))",
        R"(bytes().insert(1, 0))",
        R"(bytes().remove())",
        R"(bytes().remove("test"))",
        R"(bytes().remove(1))",
        R"(bytes().index())",
        R"(bytes().resize())",
        R"(bytes().resize(1))",
        R"(bytes().resize(-1, 1))",
        R"(bytes().resize("test", 1))",
        R"(bytes().resize(1, "test"))",
        R"(bytes()[-1])",
        R"(bytes()[0])",
        R"(bytes()[1])",
      };

      for (const auto& source : kSources) {
        runThrows(source);
      }
    }
  };
};

}  // namespace tests_vm_bytes
