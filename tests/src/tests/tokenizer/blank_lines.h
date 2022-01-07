#pragma once

#include "utils.h"

namespace tests_tokenizer_blank_lines {

inline suite _ = [] {
  "tokenizer_blank_lines"_test = [] {
    {
      constexpr auto kSource = "noop\n\nnoop";
      tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
      });
    }
    {
      constexpr auto kSource = "noop\n  \nnoop";
      tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
      });
    }
    {
      constexpr auto kSource = "noop\n\t\nnoop";
      tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
      });
    }
    {
      constexpr auto kSource = "noop\n\r\nnoop";
      tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
      });
    }
    {
      constexpr auto kSource = "noop\n# Comment\nnoop";
      tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
      });
    }
    {
      constexpr auto kSource = "\n\nnoop\n\n";
      tokenize(kSource, {
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
      });
    }
    {
      constexpr auto kSource = "\n\n";
      tokenize(kSource, {
        Token::Type::Eof
      });
    }
  };
};

}  // namespace tests_tokenizer_blank_lines
