#include "utils.h"

namespace tests_vm_closure {

inline suite _ = [] {
  "vm_closure"_test = [] {
    {
      constexpr const char* kSources[] = {
R"(
def test():
  assert(true)
test()
)",
R"(
var x = 1
def test():
  assert(x == 1)
test()
)",
R"(
block:
  var x = 1
  var y = 2
  var z = 3
  def test():
    assert(x + y + z == 6)
  test()
)",
R"(
var x = 1
var f
block:
  def test():
    assert(x == 1)
  f = test
var _1
f()
var _2
f()
)",
R"(
block:
  var x = 1
  def outer():
    def inner():
      assert(x == 1)
    inner()
  outer()
)",
R"(
var x = 1
block:
  def test1():
    var y = 2
    def test2():
      var z = 3
      def test3():
        assert(x == 1)
        assert(y == 2)
        assert(z == 3)
      test3()
      var _1
      test3()
    test2()
    var _1
    test2()
  test1()
  var _1
  test1()
)",
R"(
var x = 1
block:
  def test():
    assert(x == 1)
  test()
  var _1
  test()
  var _2
  test()
)",
R"(
var x = 1
block:
  def test(n):
    x = n
  test(2)
  assert(x == 2)
  test(3)
  assert(x == 3)
)",
      };

      for (const auto& source : kSources) {
        run(source);
      }
    }
  };
};

}  // namespace tests_vm_closure
