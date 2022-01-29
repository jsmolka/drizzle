#pragma once

#include "utils.h"

namespace tests_compiler_error {

inline suite _ = [] {
  "tests_compiler_error"_test = [] {
    {
      constexpr const char* kSources[] = {
R"(
block x:
  block x:
    noop
)",
R"(
block x:
  break y
)",
R"(
block:
  break
)",
R"(
block:
  continue
)",
      };

      for (const auto& source : kSources) {
        compileThrows<SyntaxError>(source);
      }
    }
    {
      std::array<const char*, 256> arguments{};
      std::fill(arguments.begin(), arguments.end(), "1");
      std::string source = fmt::format("test({})", fmt::join(arguments, ","));

      compileThrows<CompilerError>(source);
    }
  };
};

}  // namespace tests_parser_syntax
