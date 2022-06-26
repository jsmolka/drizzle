#include "astvisiter.h"

#include <sh/utility.h>

void AstVisiter::visit(Expr& expr) {
  static_assert(int(Expression::Type::LastEnumValue) == 16);
  switch (expr->type) {
    case Expression::Type::Assign:       visit(expr->assign); break;
    case Expression::Type::Binary:       visit(expr->binary); break;
    case Expression::Type::Call:         visit(expr->call); break;
    case Expression::Type::Get:          visit(expr->get); break;
    case Expression::Type::Group:        visit(expr->group); break;
    case Expression::Type::In:           visit(expr->in); break;
    case Expression::Type::Invoke:       visit(expr->invoke); break;
    case Expression::Type::List:         visit(expr->list); break;
    case Expression::Type::Literal:      visit(expr->literal); break;
    case Expression::Type::Map:          visit(expr->map); break;
    case Expression::Type::Range:        visit(expr->range); break;
    case Expression::Type::Set:          visit(expr->set); break;
    case Expression::Type::SubscriptGet: visit(expr->subscript_get); break;
    case Expression::Type::SubscriptSet: visit(expr->subscript_set); break;
    case Expression::Type::Unary:        visit(expr->unary); break;
    case Expression::Type::Variable:     visit(expr->variable); break;
    default:
      SH_UNREACHABLE;
      break;
  }
}

void AstVisiter::visit(Exprs& exprs) {
  for (auto& expr : exprs) {
    visit(expr);
  }
}

void AstVisiter::visit(Expression::Assign& assign) {
  visit(assign.value);
}

void AstVisiter::visit(Expression::Binary& binary) {
  visit(binary.left);
  visit(binary.right);
}

void AstVisiter::visit(Expression::Call& call) {
  visit(call.callee);
  visit(call.arguments);
}

void AstVisiter::visit(Expression::Get& get) {
  visit(get.self);
}

void AstVisiter::visit(Expression::Group& group) {
  visit(group.expression);
}

void AstVisiter::visit(Expression::In& in) {
  visit(in.self);
  visit(in.expression);
}

void AstVisiter::visit(Expression::Invoke& invoke) {
  visit(invoke.self);
  visit(invoke.arguments);
}

void AstVisiter::visit(Expression::List& list) {
  visit(list.values);
}

void AstVisiter::visit(Expression::Literal& literal) {
  sh::unused(literal);
}

void AstVisiter::visit(Expression::Map& map) {
  for (auto& [key, value] : map.pairs) {
    visit(key);
    visit(value);
  }
}

void AstVisiter::visit(Expression::Range& range) {
  visit(range.start);
  visit(range.stop);
}

void AstVisiter::visit(Expression::Set& set) {
  visit(set.self);
  visit(set.value);
}

void AstVisiter::visit(Expression::SubscriptGet& subscript_get) {
  visit(subscript_get.self);
  visit(subscript_get.expression);
}

void AstVisiter::visit(Expression::SubscriptSet& subscript_set) {
  visit(subscript_set.self);
  visit(subscript_set.expression);
  visit(subscript_set.value);
}

void AstVisiter::visit(Expression::Unary& unary) {
  visit(unary.expression);
}

void AstVisiter::visit(Expression::Variable& variable) {
  sh::unused(variable);
}

void AstVisiter::visit(Stmt& stmt) {
  static_assert(int(Statement::Type::LastEnumValue) == 14);
  switch (stmt->type) {
    case Statement::Type::Block:               visit(stmt->block); break;
    case Statement::Type::Break:               visit(stmt->break_); break;
    case Statement::Type::Class:               visit(stmt->class_); break;
    case Statement::Type::Continue:            visit(stmt->continue_); break;
    case Statement::Type::Def:                 visit(stmt->def); break;
    case Statement::Type::ExpressionStatement: visit(stmt->expression_statement); break;
    case Statement::Type::For:                 visit(stmt->for_); break;
    case Statement::Type::If:                  visit(stmt->if_); break;
    case Statement::Type::Noop:                visit(stmt->noop); break;
    case Statement::Type::Program:             visit(stmt->program); break;
    case Statement::Type::Return:              visit(stmt->return_); break;
    case Statement::Type::Switch:              visit(stmt->switch_); break;
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

void AstVisiter::visit(Statement::Class& class_) {
  visit(class_.methods);
}

void AstVisiter::visit(Statement::Continue& continue_) {
  sh::unused(continue_);
}

void AstVisiter::visit(Statement::Def& def) {
  visit(def.statements);
}

void AstVisiter::visit(Statement::ExpressionStatement& expression_statement) {
  visit(expression_statement.expression);
}

void AstVisiter::visit(Statement::For& for_) {
  visit(for_.iteree);
  visit(for_.statements);
}

void AstVisiter::visit(Statement::If& if_) {
  for (auto& branches : if_.branches) {
    visit(branches.condition);
    visit(branches.statements);
  }

  if (if_.else_) {
    visit(*if_.else_);
  }
}

void AstVisiter::visit(Statement::Noop& noop) {
  sh::unused(noop);
}

void AstVisiter::visit(Statement::Program& program) {
  visit(program.statements);
}

void AstVisiter::visit(Statement::Return& return_) {
  if (return_.expression) {
    visit(*return_.expression);
  }
}

void AstVisiter::visit(Statement::Switch& switch_) {
  visit(switch_.value);
  for (auto& case_ : switch_.cases) {
    visit(case_.values);
    visit(case_.statements);
  }

  if (switch_.default_) {
    visit(*switch_.default_);
  }
}

void AstVisiter::visit(Statement::Var& var) {
  visit(var.initializer);
}

void AstVisiter::visit(Statement::While& while_) {
  visit(while_.condition);
  visit(while_.statements);
}
