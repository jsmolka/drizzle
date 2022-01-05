#pragma once

#include "utils.h"

namespace tests_tokenizer_indentation {

inline suite _ = [] {
  "tokenizer_indentation"_test = [] {
    {
      constexpr auto kSource = R"(
noop # 1
  noop # 2
    noop # 3
  noop # 4
noop # 5
)";
      tokenize(kSource, { 
        Token::Type::Noop,  // 1
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Noop,  // 2
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Noop,  // 3
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Noop,  // 4
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Noop,  // 5
        Token::Type::NewLine,
        Token::Type::Eof 
      });
    }
    {
      constexpr auto kSource = R"(
noop # 1
  noop # 2
  noop # 3
noop # 4
)";
      tokenize(kSource, { 
        Token::Type::Noop,  // 1
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Noop,  // 2
        Token::Type::NewLine,
        Token::Type::Noop,  // 3
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Noop,  // 4
        Token::Type::NewLine,
        Token::Type::Eof 
      });
    }
    {
      constexpr auto kSource = R"(
noop # 1
  noop # 2
    noop # 3
)";
      tokenize(kSource, { 
        Token::Type::Noop,  // 1
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Noop,  // 2
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Noop,  // 3
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Dedent,
        Token::Type::Eof 
      });
    }
    {
      constexpr auto kSource = R"(
noop # 1
  noop # 2
    noop # 3
    noop # 4
)";
      tokenize(kSource, { 
        Token::Type::Noop,  // 1
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Noop,  // 2
        Token::Type::NewLine,
        Token::Type::Indent,
        Token::Type::Noop,  // 3
        Token::Type::NewLine,
        Token::Type::Noop,  // 4
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Dedent,
        Token::Type::Eof 
      });
    }
    {
      constexpr auto kSource = "\r  noop\n";
      tokenize(kSource, { 
        Token::Type::Indent,
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Dedent,
        Token::Type::Eof 
      });
    }
    {
      constexpr auto kSource = "\tnoop";
      tokenizeThrows(kSource);
    }
    {
      constexpr auto kSource = " noop";
      tokenizeThrows(kSource);
    }
    {
      constexpr auto kSource = "    noop";
      tokenizeThrows(kSource);
    }
  };
};

}  // namespace tests_tokenizer_indentation
