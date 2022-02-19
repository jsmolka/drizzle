#include "passanalysis.h"

#include <sh/ranges.h>

void PassAnalysis::run(Stmt& ast) {
  visit(ast);
}

void PassAnalysis::visit(Statement::Block& block) {
  increaseScope();
  AstVisiter::visit(block);
  decreaseScope();
}

void PassAnalysis::visit(Statement::Def& def) {
  increaseScope();
  define(def.identifier, Variable::Type::Function);
  for (const auto& argument : def.arguments) {
    define(argument);
  }
  AstVisiter::visit(def);
  decreaseScope();
  define(def.identifier, Variable::Type::Function);
}

void PassAnalysis::visit(Statement::If& if_) {
  for (auto& branch : if_.branches) {
    AstVisiter::visit(branch.condition);
    increaseScope();
    AstVisiter::visit(branch.statements);
    decreaseScope();
  }

  increaseScope();
  AstVisiter::visit(if_.else_);
  decreaseScope();
}

void PassAnalysis::visit(Statement::Var& var) {
  define(var.identifier, Variable::Type::Normal);
  assign(var.identifier, var.initializer);
}

void PassAnalysis::visit(Statement::While& while_) {
  AstVisiter::visit(while_.condition);
  increaseScope();
  AstVisiter::visit(while_.statements);
  decreaseScope();
}

void PassAnalysis::visit(Expression::Assign& assign) {
  this->assign(assign.identifier, assign.value);
}

template<typename... Args>
void PassAnalysis::define(Args&&... args) {
  variables.emplace_back(depth, std::forward<Args>(args)...);
}

void PassAnalysis::increaseScope() {
  depth++;
}

void PassAnalysis::decreaseScope() {
  while (!variables.empty() && variables.back().depth == depth) {
    variables.pop_back();
  }
  depth--;
}

PassAnalysis::Variable* PassAnalysis::resolve(const Identifier& identifier) {
  for (auto& variable : sh::reversed(variables)) {
    if (variable.identifier == identifier) {
      return &variable;
    }
  }
  return nullptr;
}

void PassAnalysis::assign(const Identifier& identifier, const Expr& value) {
  if (auto dst = resolve(identifier)) {
    auto type = Variable::Type::Normal;
    if (value->type == Expression::Type::Variable) {
      if (const auto src = resolve(value->variable.identifier)) {
        type = src->type;
      }
    }
    dst->type = type;
  }
}
