#pragma once

#include "utils.h"

namespace tests_tokenizer_comments {

inline suite _ = [] {
  "tokenizer_comments"_test = [] {
    {
      constexpr auto kSource = "# Comment";
      tokenize(kSource, {
        Token::Type::Eof
      });
    }
    {
      constexpr auto kSource = "noop# Comment";
      tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
      });
    }
    {
      constexpr auto kSource = R"(
# Comment
noop  # Comment
noop)";
      tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
      });
    }
  };
};

}  // namespace tests_tokenizer_comments
