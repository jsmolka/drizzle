TEST_CASE("parser_stmt_print_1")
{
    constexpr auto kSource = "print 1\n";

    parse(kSource, R"(program
  print
    literal 1
)");
}

TEST_CASE("parser_stmt_print_2")
{
    constexpr auto kSource = "print 1";

    parseThrows(kSource);
}
