#include "astwalker.h"

#include <sh/utility.h>

void AstWalker::walk(Expr& expr) {
  static_assert(int(Expression::Type::LastEnumValue) == 6);

  // clang-format off
  switch (expr->type) {
    case Expression::Type::Assign:   walk(expr->assign); break;
    case Expression::Type::Binary:   walk(expr->binary); break;
    case Expression::Type::Group:    walk(expr->group); break;
    case Expression::Type::Literal:  walk(expr->literal); break;
    case Expression::Type::Unary:    walk(expr->unary); break;
    case Expression::Type::Variable: walk(expr->variable); break;
    default:
      SH_UNREACHABLE;
      break;
  }
  // clang-format on
}

void AstWalker::walk(Expression::Assign& assign) {
  walk(assign.value);
}

void AstWalker::walk(Expression::Binary& binary) {
  walk(binary.left);
  walk(binary.right);
}

void AstWalker::walk(Expression::Group& group) {
  walk(group.expression);
}

void AstWalker::walk(Expression::Literal& literal) {
  sh::unused(literal);
}

void AstWalker::walk(Expression::Unary& unary) {
  walk(unary.expression);
}

void AstWalker::walk(Expression::Variable& variable) {
  sh::unused(variable);
}

void AstWalker::walk(Stmt& stmt) {
  static_assert(int(Statement::Type::LastEnumValue) == 10);

  // clang-format off
  switch (stmt->type) {
    case Statement::Type::Block:               walk(stmt->block); break;
    case Statement::Type::Break:               walk(stmt->break_); break;
    case Statement::Type::Continue:            walk(stmt->continue_); break;
    case Statement::Type::ExpressionStatement: walk(stmt->expression_statement); break;
    case Statement::Type::If:                  walk(stmt->if_); break;
    case Statement::Type::Noop:                walk(stmt->noop); break;
    case Statement::Type::Print:               walk(stmt->print); break;
    case Statement::Type::Program:             walk(stmt->program); break;
    case Statement::Type::Var:                 walk(stmt->var); break;
    case Statement::Type::While:               walk(stmt->while_); break;
    default:
      SH_UNREACHABLE;
      break;
  }
  // clang-format on
}

void AstWalker::walk(Stmts& stmts) {
  for (auto& stmt : stmts) {
    walk(stmt);
  }
}

void AstWalker::walk(Statement::Block& block) {
  walk(block.statements);
}

void AstWalker::walk(Statement::Break& break_) {
  sh::unused(break_);
}

void AstWalker::walk(Statement::Continue& continue_) {
  sh::unused(continue_);
}

void AstWalker::walk(Statement::ExpressionStatement& expression_statement) {
  walk(expression_statement.expression);
}

void AstWalker::walk(Statement::If& if_) {
  walk(if_.if_.condition);
  walk(if_.if_.statements);

  for (auto& elif : if_.elifs) {
    walk(elif.condition);
    walk(elif.statements);
  }

  walk(if_.else_);
}

void AstWalker::walk(Statement::Noop& noop) {
  sh::unused(noop);
}

void AstWalker::walk(Statement::Print& print) {
  walk(print.expression);
}

void AstWalker::walk(Statement::Program& program) {
  walk(program.statements);
}

void AstWalker::walk(Statement::Var& var) {
  walk(var.initializer);
}

void AstWalker::walk(Statement::While& while_) {
  walk(while_.condition);
  walk(while_.statements);
}
