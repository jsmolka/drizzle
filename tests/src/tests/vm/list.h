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
assert(x[0] == 1)
assert(x[1] == 2)
assert(x[2] == 3)
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
assert(x[0] == 1)
assert(x[5] == 6)
)",
R"(
var x = []
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
var x = [1, 2, 3]
assert(x.pop() == 3)
assert(x.pop() == 2)
assert(x.pop() == 1)
assert(x.size() == 0)
)",
R"(
var x = [1, 2, 3]
assert(x.remove(1) == 2)
assert(x.remove(0) == 1)
assert(x.remove(0) == 3)
assert(x.size() == 0)
)",
R"(
var x = [1, 2, 3, 1]
assert(x.index(1) == 0)
assert(x.index(2) == 1)
assert(x.index(3) == 2)
assert(x.index(4) == -1)
)",
R"(
var x = []
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
      };

      for (const auto& source : kSources) {
        run(source);
      }
    }
    {
      constexpr const char* kSources[] = {
R"([].insert())",
R"([].insert(1))",
R"([].insert("test", 0))",
R"([].insert(1, 0))",
R"([].remove())",
R"([].remove("test"))",
R"([].remove(1))",
R"([].index())",
R"([].resize())",
R"([].resize(1))",
R"([].resize("test", 1))",
      };

      for (const auto& source : kSources) {
        runThrows(source);
      }
    }
  };
};

}  // namespace tests_vm_list
