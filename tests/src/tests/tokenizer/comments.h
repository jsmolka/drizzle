#pragma once

#include "utils.h"

namespace tests_tokenizer_comments {

inline suite _ = [] {
  "tokenizer_comments"_test = [] {
    constexpr const char* kSources[] = {
      "noop#Comment",
      "noop # Comment",
      "noop  #  Comment",
    };

    for (const auto& source : kSources) {
      tokenize(source, {
        Token::Type::Noop,
        Token::Type::NewLine,
        Token::Type::Eof
      });
    }
  };
};

}  // namespace tests_tokenizer_comments
