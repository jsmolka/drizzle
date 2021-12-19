#include "expression.h"

#include <sh/utility.h>

Expression::~Expression() {
  // clang-format off
  switch (type) {
    case Type::Assign: assign.~Assign(); break;
    case Type::Binary: binary.~Binary(); break;
    case Type::Group: group.~Group(); break;
    case Type::Literal: literal.~Literal(); break;
    case Type::Unary: unary.~Unary(); break;
    case Type::Variable: variable.~Variable(); break;
    default:
      SH_UNREACHABLE;
      break;
  }
  // clang-format on
}
