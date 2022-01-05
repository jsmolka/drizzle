#pragma once

#include "utils.h"

namespace tests_tokenizer_number {

inline suite _ = [] {
  "tokenizer_number"_test = [] {
    {
      constexpr auto kSource = "0b0";
      tokenizeValue<dzint>(kSource, 0b0);
    }
    {
      constexpr auto kSource = "0b01001";
      tokenizeValue<dzint>(kSource, 0b01001);
    }
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
      constexpr auto kSource = "0x0";
      tokenizeValue<dzint>(kSource, 0x0);
    }
    {
      constexpr auto kSource = "0xabcdef";
      tokenizeValue<dzint>(kSource, 0xabcdef);
    }
    {
      constexpr auto kSource = "0xABCDEF";
      tokenizeValue<dzint>(kSource, 0xABCDEF);
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
      constexpr auto kSource = "100";
      tokenizeValue<dzint>(kSource, 100);
    }
    {
      constexpr auto kSource = "01";
      tokenizeThrows(kSource);
    }
    {
      constexpr auto kSource = "1.1";
      tokenizeValue<dzfloat>(kSource, 1.1);
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
