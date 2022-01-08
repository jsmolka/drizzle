#pragma once

#include "utils.h"

namespace tests_tokenizer_string {

inline suite _ = [] {
  "tokenizer_string"_test = [] {
    {
      constexpr auto kSource = R"("""string""")";
      tokenizeValue<std::string>(kSource, "string");
    }
    {
      constexpr auto kSource = R"("""string
""")";
      tokenizeValue<std::string>(kSource, "string\n");
    }
    {
      constexpr auto kSource = R"("""string\""")";
      tokenizeValue<std::string>(kSource, "string\\");
    }
    {
      constexpr auto kSource = R"("""string """)";
      tokenizeValue<std::string>(kSource, "string ");
    }
    {
      constexpr auto kSource = R"("string")";
      tokenizeValue<std::string>(kSource, "string");
    }
    {
      constexpr auto kSource = R"("\\ \" \a \b \f \n \r \t \v")";
      tokenizeValue<std::string>(kSource, "\\ \" \a \b \f \n \r \t \v");
    }
    {
      constexpr auto kSource = R"("
")";
      tokenizeThrows(kSource);
    }
    {
      std::vector<char> chars;
      for (int c = 0; c < 256; ++c) {
        switch (c) {
          case '\\':
          case '"':
          case 'a':
          case 'b':
          case 'f':
          case 'n':
          case 'r':
          case 't':
          case 'v':
            continue;
        }
        chars.push_back(c);
      }

      for (const auto& c : chars) {
        const auto source = fmt::format(R"("\{}")", c);
        tokenizeThrows(source);
      }
    }
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
