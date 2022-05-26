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
R"(
# This one used to cause a segfault
class Vector:
  noop
Vector()
class Point:
  def init():
    noop
)",
R"(
# This one used to cause a segfault
class Test:
  def test():
    return 1
  def test():
    return 2
var t = Test()
assert(t.test() == 2)
)",
R"(
class Test:
  noop
var t = Test()
t.prop = 1
assert(t.prop == 1)
assert(t["prop"] == 1)
t["prop2"] = 2
assert(t.prop2 == 2)
assert(t["prop2"] == 2)
)",
R"(
class Test:
  def test():
    noop
var t1 = Test()
var t2 = Test()
assert(t1.test == t1.test)
assert(t1.test != t2.test)
)",
R"(
var f = [].push
assert(f(1) == 1)
assert(f(1) == 2)
assert(f(1) == 3)
)",
R"(
class Test:
  def test():
    noop
var t = Test()
assert("test" in t)
assert(!("test2" in t))
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
R"(
class Test:
  def test():
    noop
assert(1 in Test())
)",
      };

      for (const auto& source : kSources) {
        runThrows(source);
      }
    }
  };
};

}  // namespace tests_vm_class
