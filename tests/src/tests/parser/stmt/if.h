#pragma once

#include "utils.h"

namespace tests_parser_stmt_if {

inline suite _ = [] {
  "parser_stmt_if"_test = [] {
    {
      constexpr auto kSource = R"(
if true:
  noop)";
      constexpr auto kExpect = R"(program
  if
    literal true
    noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
if 1 == 1:
  noop)";
      constexpr auto kExpect = R"(program
  if
    binary ==
      literal 1
      literal 1
    noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
if 1 == 1:
  noop
  noop)";
      constexpr auto kExpect = R"(program
  if
    binary ==
      literal 1
      literal 1
    noop
    noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
if true:
  noop
elif true:
  noop)";
      constexpr auto kExpect = R"(program
  if
    literal true
    noop
  elif
    literal true
    noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
if true:
  noop
elif 1 == 1:
  noop)";
      constexpr auto kExpect = R"(program
  if
    literal true
    noop
  elif
    binary ==
      literal 1
      literal 1
    noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
if true:
  noop
elif 1 == 1:
  noop
  noop)";
      constexpr auto kExpect = R"(program
  if
    literal true
    noop
  elif
    binary ==
      literal 1
      literal 1
    noop
    noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
if true:
  noop
elif true:
  noop
else:
  noop)";
      constexpr auto kExpect = R"(program
  if
    literal true
    noop
  elif
    literal true
    noop
  else
    noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
if true:
  noop
elif true:
  noop
else:
  noop
  noop)";
      constexpr auto kExpect = R"(program
  if
    literal true
    noop
  elif
    literal true
    noop
  else
    noop
    noop)";
      parse(kSource, kExpect);
    }
    {
      constexpr auto kSource = R"(
if:
  noop)";
      parseThrows(kSource);
    }
    {
      constexpr auto kSource = R"(
if true
  noop)";
      parseThrows(kSource);
    }
    {
      constexpr auto kSource = R"(
if true:)";
      parseThrows(kSource);
    }
    {
      constexpr auto kSource = R"(
if true: pass)";
      parseThrows(kSource);
    }
    {
      constexpr auto kSource = R"(
if true:
  noop
elif true
  noop)";
      parseThrows(kSource);
    }
    {
      constexpr auto kSource = R"(
if true:
  noop
elif true:)";
      parseThrows(kSource);
    }
    {
      constexpr auto kSource = R"(
if true:
  noop
elif true: noop)";
      parseThrows(kSource);
    }
    {
      constexpr auto kSource = R"(
if true:
  noop
elif true:
  noop
else
  noop)";
      parseThrows(kSource);
    }
    {
      constexpr auto kSource = R"(
if true:
  noop
elif true:
  noop
else:)";
      parseThrows(kSource);
    }
    {
      constexpr auto kSource = R"(
if true:
  noop
elif true:
  noop
elif: noop)";
      parseThrows(kSource);
    }
    {
      constexpr auto kSource = R"(elif)";
      parseThrows(kSource);
    }
    {
      constexpr auto kSource = R"(else)";
      parseThrows(kSource);
    }
    {
      constexpr auto kSource = R"(
else true:
  pass)";
      parseThrows(kSource);
    }
  };
};

}  // namespace tests_parser_stmt_if
