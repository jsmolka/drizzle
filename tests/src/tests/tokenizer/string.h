#pragma once

#include "utils.h"

namespace tests_tokenizer_string {

inline suite _ = [] {
  "tokenize_string"_test = [] {
    {
      constexpr const char* kSources[] = {
        R"("")",
        R"("test")",
        R"("\n")",
        R"("\t")",
        R"("\r")",
        R"("\\")",
        R"("\"")",
        R"("""""")",
        R"("""test""")",
      };

      for (const auto& source : kSources) {
        tokenize(source, {
          Token::Type::String,
          Token::Type::NewLine,
          Token::Type::Eof,
        });
      }
    }
    {
      constexpr const char* kSources[] = {
        R"(")",
        R"(""")",
        R"("""")",
        R"("string)",
        R"("""string")",
        R"("""string"")",
      };

      for (const auto& source : kSources) {
        tokenizeThrows(source);
      }
    }
  };
};

}  // namespace tests_tokenizer_string
