#pragma

#include "utils.h"

namespace tests_vm_map {

inline suite _ = [] {
  "vm_map"_test = [] {
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
assert(len(m) == 2)
m.clear()
assert(len(m) == 0)
)",
R"(
var m = {1: 1}
assert(m[1] == 1)
assert(m[1.0] == 1)
assert(m[true] == 1)
assert(1 in m)
)",
R"(
var m = {[]: 1}
assert(len(m) == 1)
)",
R"(
var m = {null: 1}
assert(null in m)
)",
R"(
var l1 = [1, 2, 3]
var l2 = [1, 2, 3]
var m = {l1: 1}
assert(l1 in m)
assert(!(l2 in m))
)",
    };

    for (const auto& source : kSources) {
      run(source);
    }
  };
};

}  // namespace tests_vm_map
