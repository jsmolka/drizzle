#include "passanalysis.h"

#include <sh/ranges.h>

#include "error.h"

void PassAnalysis::run(Stmt& ast) {
  visit(ast);
}

void PassAnalysis::visit(Statement::Block& block) {
  increase(Level::Block);
  AstVisiter::visit(block);
  decrease();
}

void PassAnalysis::visit(Statement::Def& def) {
  increase(Level::Function);
  define(def.identifier);
  for (const auto& argument : def.arguments) {
    define(argument);
  }
  AstVisiter::visit(def);
  const auto level = decrease();
  define(def.identifier, level == Level::Closure
    ? Variable::Type::Closure
    : Variable::Type::Function);
}

void PassAnalysis::visit(Statement::If& if_) {
  for (auto& branch : if_.branches) {
    AstVisiter::visit(branch.condition);
    increase(Level::Branch);
    AstVisiter::visit(branch.statements);
    decrease();
  }
  increase(Level::Branch);
  AstVisiter::visit(if_.else_);
  decrease();
}

void PassAnalysis::visit(Statement::Var& var) {
  define(var.identifier);
}

void PassAnalysis::visit(Statement::While& while_) {
  AstVisiter::visit(while_.condition);
  increase(Level::Loop);
  AstVisiter::visit(while_.statements);
  decrease();
}

void PassAnalysis::visit(Expression::Call& call) {
  callee = true;
  AstVisiter::visit(call.callee);
  callee = false;
  AstVisiter::visit(call.arguments);
}

void PassAnalysis::visit(Expression::Variable& variable) {
  resolve(variable.identifier);
}

void PassAnalysis::increase(Level level) {
  scope.push(level);
}

auto PassAnalysis::decrease() -> Level {
  while (!variables.empty() && variables.top().depth == scope.size()) {
    variables.pop();
  }
  return scope.pop_value();
}

template<typename... Args>
void PassAnalysis::define(Args&&... args) {
  variables.emplace(scope.size(), std::forward<Args>(args)...);
}

void PassAnalysis::resolve(const Identifier& identifier) {
  for (auto& variable : sh::reversed(variables)) {
    if (variable.identifier == identifier && variable.depth > 0) {
      if (variable.type == Variable::Type::Closure && !callee) {
        throw SyntaxError(identifier.location, "cannot dereference function that captures local variables");
      }

      // Todo: this could probably use variable.type
      for (auto& level : sh::range(scope.begin() + variable.depth, scope.end())) {
        if (level == Level::Function) {
          level = Level::Closure;
        }
      }
    }
  }
}
