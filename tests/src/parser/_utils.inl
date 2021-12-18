void parse(const std::string& source, const std::string& expect) {
  const auto tokens = Tokenizer().tokenize(source);
  auto ast = Parser().parse(tokens);
  REQUIRE(fmt::format("{}", ast) == expect);
}

void parseThrows(const std::string& source) {
  const auto tokens = Tokenizer().tokenize(source);
  REQUIRE_THROWS_AS(Parser().parse(tokens), SyntaxError);
}
