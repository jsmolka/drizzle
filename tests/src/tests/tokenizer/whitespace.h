#pragma once

#include "utils.h"

namespace tests_tokenizer_whitespace {

inline suite _ = [] {
  "tokenizer_whitespace"_test = [] {
    {
      constexpr auto kSource = "noop noop";
      tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
      });
    }
    {
      constexpr auto kSource = "noop\tnoop";
      tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
      });
    }
    {
      constexpr auto kSource = "noop\rnoop";
      tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
      });
    }
  };
};

}  // namespace tests_tokenizer_whitespace
