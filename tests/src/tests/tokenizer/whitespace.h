#pragma once

#include "utils.h"

namespace tests_tokenizer_whitespace {

inline suite _ = [] {
  "tokenizer_whitespace"_test = [] {
    constexpr const char* kSources[] = {
      "noop noop",
      "noop  noop",
      "noop   noop",
      "noop\tnoop",
      "noop\t\tnoop",
      "noop\t\t\tnoop",
      "noop\rnoop",
      "noop\r\rnoop",
      "noop\r\r\rnoop",
    };

    for (const auto& source : kSources) {
      tokenize(source, {
        Token::Type::Noop,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
      });
    }
  };
};

}  // namespace tests_tokenizer_whitespace
