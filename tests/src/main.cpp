#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "astprinter.h"
#include "errors.h"
#include "expression.h"
#include "parser.h"
#include "statement.h"
#include "tokenizer.h"

#include "parser/_tests.inl"

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
