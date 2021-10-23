#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <shell/ranges.h>

#include "astwalker.h"
#include "errors.h"
#include "expression.h"
#include "parser.h"
#include "statement.h"
#include "tokenizer.h"

#include "tests_parser.inl"
#include "tests_parser_expr_binary.inl"
#include "tests_tokenizer.inl"
#include "tests_tokenizer_blank_lines.inl"
#include "tests_tokenizer_comments.inl"
#include "tests_tokenizer_eof.inl"
#include "tests_tokenizer_identifiers.inl"
#include "tests_tokenizer_indentation.inl"
#include "tests_tokenizer_number.inl"
#include "tests_tokenizer_string.inl"
#include "tests_tokenizer_types.inl"
#include "tests_tokenizer_whitespace.inl"
