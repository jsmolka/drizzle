#pragma once

#include "utils.h"

namespace tests_tokenizer_number {

inline suite _ = [] {
  "tokenizer_number"_test = [] {
    {
      constexpr auto kSource = "0b";
      tokenizeThrows(kSource);
    }
    {
      constexpr auto kSource = "0b2";
      tokenizeThrows(kSource);
    }
    {
      constexpr auto kSource = "0b_";
      tokenizeThrows(kSource);
    }
    {
      constexpr auto kSource = "0x";
      tokenizeThrows(kSource);
    }
    {
      constexpr auto kSource = "0xn";
      tokenizeThrows(kSource);
    }
    {
      constexpr auto kSource = "0x_";
      tokenizeThrows(kSource);
    }
    {
      constexpr auto kSource = "01";
      tokenizeThrows(kSource);
    }
    {
      constexpr auto kSource = "01.1";
      tokenizeThrows(kSource);
    }
    {
      constexpr auto kSource = "01.x1";
      tokenizeThrows(kSource);
    }
  };
};

}  // namespace tests_tokenizer_number
