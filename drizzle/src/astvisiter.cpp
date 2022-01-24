#include "astvisiter.h"

#include <sh/utility.h>

void AstVisiter::visit(Expr& expr) {
  static_assert(int(Expression::Type::LastEnumValue) == 6);

  switch (expr->type) {
    case Expression::Type::Assign:   visit(expr->assign); break;
    case Expression::Type::Binary:   visit(expr->binary); break;
    case Expression::Type::Group:    visit(expr->group); break;
    case Expression::Type::Literal:  visit(expr->literal); break;
    case Expression::Type::Unary:    visit(expr->unary); break;
    case Expression::Type::Variable: visit(expr->variable); break;
    default:
      SH_UNREACHABLE;
      break;
  }
}

void AstVisiter::visit(Expression::Assign& assign) {
  visit(assign.value);
}

void AstVisiter::visit(Expression::Binary& binary) {
  visit(binary.left);
  visit(binary.right);
}

void AstVisiter::visit(Expression::Group& group) {
  visit(group.expression);
}

void AstVisiter::visit(Expression::Literal& literal) {
  sh::unused(literal);
}

void AstVisiter::visit(Expression::Unary& unary) {
  visit(unary.expression);
}

void AstVisiter::visit(Expression::Variable& variable) {
  sh::unused(variable);
}

void AstVisiter::visit(Stmt& stmt) {
  static_assert(int(Statement::Type::LastEnumValue) == 11);

  switch (stmt->type) {
    case Statement::Type::Block:               visit(stmt->block); break;
    case Statement::Type::Break:               visit(stmt->break_); break;
    case Statement::Type::Continue:            visit(stmt->continue_); break;
    case Statement::Type::Def:                 visit(stmt->def.statements); break;
    case Statement::Type::ExpressionStatement: visit(stmt->expression_statement); break;
    case Statement::Type::If:                  visit(stmt->if_); break;
    case Statement::Type::Noop:                visit(stmt->noop); break;
    case Statement::Type::Print:               visit(stmt->print); break;
    case Statement::Type::Program:             visit(stmt->program); break;
    case Statement::Type::Var:                 visit(stmt->var); break;
    case Statement::Type::While:               visit(stmt->while_); break;
    default:
      SH_UNREACHABLE;
      break;
  }
}

void AstVisiter::visit(Stmts& stmts) {
  for (auto& stmt : stmts) {
    visit(stmt);
  }
}

void AstVisiter::visit(Statement::Block& block) {
  visit(block.statements);
}

void AstVisiter::visit(Statement::Break& break_) {
  sh::unused(break_);
}

void AstVisiter::visit(Statement::Continue& continue_) {
  sh::unused(continue_);
}

void AstVisiter::visit(Statement::ExpressionStatement& expression_statement) {
  visit(expression_statement.expression);
}

void AstVisiter::visit(Statement::If& if_) {
  for (auto& branches : if_.branches) {
    visit(branches.condition);
    visit(branches.statements);
  }
  visit(if_.else_);
}

void AstVisiter::visit(Statement::Noop& noop) {
  sh::unused(noop);
}

void AstVisiter::visit(Statement::Print& print) {
  visit(print.expression);
}

void AstVisiter::visit(Statement::Program& program) {
  visit(program.statements);
}

void AstVisiter::visit(Statement::Var& var) {
  visit(var.initializer);
}

void AstVisiter::visit(Statement::While& while_) {
  visit(while_.condition);
  visit(while_.statements);
}
