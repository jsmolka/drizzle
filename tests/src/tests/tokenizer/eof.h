#pragma once

#include "utils.h"

namespace tests_tokenizer_eof {

inline suite _ = [] {
  "tokenizer_eof"_test = [] {
    constexpr const char* kSources[] = {
      "",
      " ",
      "  ",
      "#Comment",
      "# Comment",
      "\n\n",
      "\r\r",
      "\t\t",
    };
    for (const auto source : kSources) {
      tokenize(source, {
        Token::Type::Eof,
      });
    }
  };
};

}  // namespace tests_tokenizer_eof
