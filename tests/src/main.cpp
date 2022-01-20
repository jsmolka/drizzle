#include "utils.h"

#include "tests/parser/syntax.h"

static_assert(int(Expression::Type::LastEnumValue) == 6);
#include "tests/parser/expr/assign.h"
#include "tests/parser/expr/binary.h"
#include "tests/parser/expr/group.h"
#include "tests/parser/expr/literal.h"
#include "tests/parser/expr/precedence.h"
#include "tests/parser/expr/unary.h"
#include "tests/parser/expr/variable.h"

static_assert(int(Statement::Type::LastEnumValue) == 10);
#include "tests/parser/stmt/block.h"
#include "tests/parser/stmt/break.h"
#include "tests/parser/stmt/continue.h"
#include "tests/parser/stmt/expression_statement.h"
#include "tests/parser/stmt/if.h"
#include "tests/parser/stmt/noop.h"
#include "tests/parser/stmt/print.h"
#include "tests/parser/stmt/program.h"
#include "tests/parser/stmt/var.h"
#include "tests/parser/stmt/while.h"

#include "tests/tokenizer/blank_lines.h"
#include "tests/tokenizer/comments.h"
#include "tests/tokenizer/eof.h"
#include "tests/tokenizer/identifiers.h"
#include "tests/tokenizer/indentation.h"
#include "tests/tokenizer/number.h"
#include "tests/tokenizer/string.h"
#include "tests/tokenizer/types.h"
#include "tests/tokenizer/whitespace.h"

auto main() -> int {
  return 0;
}
