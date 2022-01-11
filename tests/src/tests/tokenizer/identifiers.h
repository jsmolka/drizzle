#pragma once

#include "utils.h"

namespace tests_tokenizer_identifiers {

inline suite _ = [] {
  "tokenizer_identifiers"_test = [] {
    {
      constexpr auto kSource = "break breaking";
      tokenize(kSource, {
        Token::Type::Break,
        Token::Type::Identifier,
        Token::Type::NewLine,
        Token::Type::Eof
      });
    }
    {
      constexpr auto kSource = "continue continued";
      tokenize(kSource, {
        Token::Type::Continue,
        Token::Type::Identifier,
        Token::Type::NewLine,
        Token::Type::Eof
      });
    }
  };
};

}  // namespace tests_tokenizer_identifiers
