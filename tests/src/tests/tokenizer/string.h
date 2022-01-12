#pragma once

#include "utils.h"

namespace tests_tokenizer_string {

inline suite _ = [] {
  "tokenizer_string"_test = [] {
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
      for (int c = 0; c < 256; ++c) {
        const auto source = fmt::format(R"("\{}")", char(c));
        switch (c) {
          case '\\':
          case '\"':
          case 'n':
          case 't':
          case 'r':
            tokenize(source, {
              Token::Type::String,
              Token::Type::NewLine,
              Token::Type::Eof,
            });
            break;
          default:
            tokenizeThrows(source);
            break;
        }
      }
    }
    {
      constexpr const char* kSources[] = {
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
