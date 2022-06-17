#include "utils.h"

#include "tests/compiler/error.h"
#include "tests/parser/error.h"

static_assert(int(Expression::Type::LastEnumValue) == 16);
#include "tests/parser/expr/assign.h"
#include "tests/parser/expr/binary.h"
#include "tests/parser/expr/call.h"
#include "tests/parser/expr/get.h"
#include "tests/parser/expr/group.h"
#include "tests/parser/expr/in.h"
#include "tests/parser/expr/invoke.h"
#include "tests/parser/expr/list.h"
#include "tests/parser/expr/literal.h"
#include "tests/parser/expr/map.h"
#include "tests/parser/expr/precedence.h"
#include "tests/parser/expr/range.h"
#include "tests/parser/expr/set.h"
#include "tests/parser/expr/subscript.h"
#include "tests/parser/expr/unary.h"
#include "tests/parser/expr/variable.h"

static_assert(int(Statement::Type::LastEnumValue) == 14);
#include "tests/parser/stmt/block.h"
#include "tests/parser/stmt/break.h"
#include "tests/parser/stmt/class.h"
#include "tests/parser/stmt/continue.h"
#include "tests/parser/stmt/def.h"
#include "tests/parser/stmt/expression_statement.h"
#include "tests/parser/stmt/for.h"
#include "tests/parser/stmt/if.h"
#include "tests/parser/stmt/noop.h"
#include "tests/parser/stmt/program.h"
#include "tests/parser/stmt/return.h"
#include "tests/parser/stmt/switch.h"
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

#include "tests/vm/assert.h"
#include "tests/vm/binary.h"
#include "tests/vm/block.h"
#include "tests/vm/branch.h"
#include "tests/vm/call.h"
#include "tests/vm/class.h"
#include "tests/vm/for.h"
#include "tests/vm/globals.h"
#include "tests/vm/invoke.h"
#include "tests/vm/list.h"
#include "tests/vm/loop.h"
#include "tests/vm/map.h"
#include "tests/vm/range.h"
#include "tests/vm/scope.h"
#include "tests/vm/string.h"
#include "tests/vm/type.h"
#include "tests/vm/unary.h"

auto main() -> int {
  return 0;
}
