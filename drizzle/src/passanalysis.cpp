#include "passanalysis.h"

#include <sh/fmt.h>
#include <sh/ranges.h>

void PassAnalysis::run(Stmt& ast) {
  visit(ast);
}

void PassAnalysis::visit(Statement::Block& block) {
  const auto size = variables.size();
  AstVisiter::visit(block);
  variables.resize(size);
}

void PassAnalysis::visit(Statement::Def& def) {
  const auto size = variables.size();
  AstVisiter::visit(def);
  variables.resize(size);

  variables.push_back(Variable{
    .type = Variable::Type::Function,
    .identifier = def.identifier
  });
  fmt::print("var {}\n", def.identifier);
}

void PassAnalysis::visit(Statement::If& if_) {
  for (auto& branch : if_.branches) {
    const auto size = variables.size();
    AstVisiter::visit(branch.condition);
    AstVisiter::visit(branch.statements);
    variables.resize(size);
  }
  const auto size = variables.size();
  AstVisiter::visit(if_.else_);
  variables.resize(size);
}

void PassAnalysis::visit(Statement::Var& var) {
  auto type = Variable::Type::Normal;
  if (var.initializer->type == Expression::Type::Variable) {
    if (auto variable = resolve(var.initializer->variable.identifier)) {
      type = variable->type;
    }
  }
  variables.push_back(Variable{
    .type = type,
    .identifier = var.identifier
  });
  fmt::print("var {}\n", var.identifier);
}

void PassAnalysis::visit(Statement::While& while_) {
  const auto size = variables.size();
  AstVisiter::visit(while_);
  variables.resize(size);
}

void PassAnalysis::visit(Expression::Assign& assign) {
  if (auto variable = resolve(assign.identifier)) {
    auto type = Variable::Type::Normal;
    if (assign.value->type == Expression::Type::Variable) {
      if (auto value = resolve(assign.value->variable.identifier)) {
        type = value->type;
      }
    }
    variable->type = type;
  }
}

PassAnalysis::Variable* PassAnalysis::resolve(const Identifier& identifier) {
  for (auto& variable : sh::reversed(variables)) {
    if (variable.identifier == identifier) {
      return &variable;
    }
  }
  return nullptr;
}
