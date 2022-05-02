#pragma

#include "utils.h"

namespace tests_vm_class {

inline suite _ = [] {
  "vm_class"_test = [] {
    {
      constexpr const char* kSources[] = {
R"(
class Point:
  noop
var p = Point()
assert(p.x == null)
assert(p.y == null)
)",
R"(
class Point:
  noop
var p = Point()
p.x = 0
p.y = 1
assert(p.x == 0)
assert(p.y == 1)
)",
R"(
class Point:
  def init(x, y):
    this.x = x
    this.y = y
var p = Point(0, 1)
assert(p.x == 0)
assert(p.y == 1)
)",
R"(
class Point:
  def init(x, y):
    this.x = x
    if y == 1:
      return
    this.y = y
var p = Point(0, 1)
assert(p.x == 0)
assert(p.y == null)
)",
R"(
class Point:
  def init(x, y):
    this.x = x
    this.y = y
  def add(x, y):
    this.x = this.x + x
    this.y = this.y + y
    return this.x + this.y
var p = Point(0, 1)
assert(p.x == 0)
assert(p.y == 1)
assert(p.add(2, 3) == 6)
assert(p.x == 2)
assert(p.y == 4)
)",
R"(
class Point:
  def init(x, y):
    this.x = x
    this.y = y
  def add(x, y):
    var p = Point(x, y)
    this.x = this.x + p.x
    this.y = this.y + p.y
    return this.x + this.y
var p = Point(0, 1)
assert(p.x == 0)
assert(p.y == 1)
assert(p.add(2, 3) == 6)
assert(p.x == 2)
assert(p.y == 4)
)",
R"(
class Point:
  def init(x, y):
    this.x = x
    this.y = y
  def add():
    return this.x + this.y
var p = Point(0, 1)
var m = p.add
assert(m != null)
assert(m() == 1)
)",
R"(
def adder(x, y):
  class Point:
    def init(x, y):
      this.x = x
      this.y = y
    def add():
      return this.x + this.y
  var p = Point(x, y)
  return p.add
assert(adder(0, 1)() == 1)
)",
      };

      for (const auto& source : kSources) {
        run(source);
      }
    }
    {
      constexpr const char* kSources[] = {
R"(
class Point:
  noop
Point(1, 2)
)",
R"(
class Point:
  def init(x, y):
    this.x = x
    this.y = y
Point(0)
)",
      };

      for (const auto& source : kSources) {
        runThrows(source);
      }
    }
  };
};

}  // namespace tests_vm_class