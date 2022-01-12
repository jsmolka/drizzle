#pragma once

#include "utils.h"

namespace tests_tokenizer_blank_lines {

inline suite _ = [] {
  "tokenizer_blank_lines"_test = [] {
    {
      constexpr const char* kSources[] = {
        "noop\nnoop",
        "noop\n\nnoop",
        "noop\n\n\nnoop",
        "noop\n \nnoop",
        "noop\n  \nnoop",
        "noop\n\t\nnoop",
        "noop\n\t\t\nnoop",
        "noop\n\r\nnoop",
        "noop\n\r\r\nnoop",
        "noop\n# Comment\nnoop",
        "noop\n# Comment\n#One more\nnoop",
      };

      for (const auto& source : kSources) {
        tokenize(source, {
          Token::Type::Noop,
          Token::Type::NewLine,
          Token::Type::Noop,
          Token::Type::NewLine,
          Token::Type::Eof,
        });
      }
    }
    {
      constexpr const char* kSources[] = {
        "\n",
        "\n\n",
        "\n\n\n",
        "\n \n",
        "\n  \n",
        "\n\t\n",
        "\n\t\t\n",
        "\n\r\n",
        "\n\r\r\n",
        "\n# Comment\n",
        "\n# Comment\n#One more\n",
      };

      for (const auto& source : kSources) {
        tokenize(source, {
          Token::Type::Eof,
        });
      }
    }
  };
};

}  // namespace tests_tokenizer_blank_lines
