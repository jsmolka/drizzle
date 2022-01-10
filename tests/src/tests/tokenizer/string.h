#pragma once

#include "utils.h"

namespace tests_tokenizer_string {

inline suite _ = [] {
  "tokenizer_string"_test = [] {
    {
      constexpr auto kSource = R"("string)";
      tokenizeThrows(kSource);
    }
    {
      constexpr auto kSource = R"("""string)";;
      tokenizeThrows(kSource);
    }
  };
};

}  // namespace tests_tokenizer_string
