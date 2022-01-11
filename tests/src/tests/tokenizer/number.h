#pragma once

#include "utils.h"

namespace tests_tokenizer_number {

inline suite _ = [] {
  "tokenizer_number_integer"_test = [] {
    {
      constexpr const char* kSources[] = {
        "0b0",
        "0b10",
        "0b010",
        "0x0",
        "0x10",
        "0x010",
        "0xabcdef",
        "0xABCDEF",
        "0x0123456789",
        "0",
        "123",
        "123456789",
      };

      for (const auto& source : kSources) {
        tokenize(source, {
          Token::Type::Integer,
          Token::Type::NewLine,
          Token::Type::Eof,
        });
      }
    }
    {
      constexpr const char* kSources[] = {
        "0b",
        "0b2",
        "0x",
        "0xx",
        "0xX",
        "00",
        "10x",
      };

      for (const auto& source : kSources) {
        tokenizeThrows(source);
      }
    }
  };

  "tokenize_number_float"_test = [] {
    {
      constexpr const char* kSources[] = {
        "0.0",
        "0.123456789",
        "12343.56789",
      };

      for (const auto& source : kSources) {
        tokenize(source, {
          Token::Type::Float,
          Token::Type::NewLine,
          Token::Type::Eof,
        });
      }
    }
    {
      constexpr const char* kSources[] = {
        "0.",
        "0.X",
      };

      for (const auto& source : kSources) {
        tokenizeThrows(source);
      }
    }
  };

  "tokenize_number_leading_zero"_test = [] {
    {
      tokenize("(0)", {
        Token::Type::ParenLeft,
        Token::Type::Integer,
        Token::Type::ParenRight,
        Token::Type::NewLine,
        Token::Type::Eof,
      });
    }
    {
      constexpr const char* kSources[] = {
        "00",
        "0.",
        "00.",
        "0a"
      };

      for (const auto& source : kSources) {
        tokenizeThrows(source);
      }
    }
  };
};

}  // namespace tests_tokenizer_number
