void tokenize(const std::string& source, const std::vector<Token::Type>& expected) {
  const auto tokens = Tokenizer().tokenize(source);
  const auto size = std::min(tokens.size(), expected.size());

  for (int i = 0; i < size; ++i)
    REQUIRE(tokens[i].type == expected[i]);

  REQUIRE(tokens.size() == expected.size());
}

template <typename T>
void tokenizeValue(const std::string& source, const T& value) {
  static_assert(dz_int<T> || dz_float<T> || std::is_same_v<T, std::string>);

  const auto tokens = Tokenizer().tokenize(source);
  for (const auto& token : tokens) {
    if constexpr (dz_int<T>) {
      if (token.type != Token::Type::Integer) continue;
    }
    if constexpr (dz_float<T>) {
      if (token.type != Token::Type::Float) continue;
    }
    if constexpr (std::is_same_v<T, std::string>) {
      if (token.type != Token::Type::String) continue;
    }

    REQUIRE(std::get<T>(token.value) == value);
    return;
  }
  REQUIRE(false);
}

void tokenizeThrows(const std::string& source) {
  REQUIRE_THROWS_AS(Tokenizer().tokenize(source), SyntaxError);
}
