#pragma once

#include "utils.h"

namespace tests_parser_stmt_define {

inline suite _ = [] {
  "parser_stmt_def"_test = [] {
    {
      constexpr auto kSource = R"(
def test():
  noop
)";
      constexpr auto kExpect = R"(program
  def test
    noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
def test(x, y):
  var z = x + y
)";
      constexpr auto kExpect = R"(program
  def test x, y
    var z
      binary +
        variable x
        variable y)";
      parse(kSource, kExpect);
    }
    {
      constexpr const char* kSources[] = {
R"(
def test:
  noop
)",
R"(
def test(:
  noop
)",
R"(
def test):
  noop
)",
R"(
def test(x,):
  noop
)",
R"(
def test(x, var):
  noop
)",
R"(
def test()
  noop
)",
R"(
def test(): noop
)",
R"(
def test():
pass
)",
      };

      for (const auto& source : kSources) {
        parseThrows(source);
      }
    }
  };
};

}  // tests_parser_stmt_block
