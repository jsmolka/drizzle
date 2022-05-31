#pragma

#include "utils.h"

namespace tests_vm_map {

inline suite _ = [] {
  "vm_map"_test = [] {
    {
      constexpr const char* kSources[] = {
R"(
var m = {"test": 1}
assert(m["test"] == 1)
assert(m.get("test") == 1)
assert("test" in m)
assert(m.has("test"))
)",
R"(
var m = {}
m.set("test", 1)
assert(m["test"] == 1)
assert(m.get("test") == 1)
assert("test" in m)
assert(m.has("test"))
)",
R"(
var m = {}
m["test"] = 1
assert(m["test"] == 1)
assert(m.get("test") == 1)
assert("test" in m)
assert(m.has("test"))
)",
R"(
var m = {"test": 2, "test": 1}
assert(m["test"] == 1)
assert(m.get("test") == 1)
assert("test" in m)
assert(m.has("test"))
)",
R"(
var m = {"test1": 1, "test2": 2}
assert(m.size() == 2)
m.clear()
assert(m.size() == 0)
)",
R"(
var m = {1: 1}
assert(m[1] == 1)
assert(m[1.0] == 1)
assert(m[true] == 1)
assert(1 in m)
)",
      };

      for (const auto& source : kSources) {
        run(source);
      }
    }
    {
      constexpr const char* kSources[] = {
R"(
var m = {[]: 1}
)",
R"(
var m = {null: 1}
)",
      };

      for (const auto& source : kSources) {
        runThrows(source);
      }
    }
  };
};

}  // namespace tests_vm_map
