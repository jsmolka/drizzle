#include "passanalysis.h"

#include <sh/ranges.h>

#include "error.h"

void PassAnalysis::run(const Stmt& ast) {
  visit(const_cast<Stmt&>(ast));
}

void PassAnalysis::visit(Statement::Block& block) {
  increase();
  AstVisiter::visit(block);
  decrease();
}

void PassAnalysis::visit(Statement::Def& def) {
  functions.push(Type::Function);
  increase();
  define(def.identifier, Type::Function);
  for (const auto& argument : def.arguments) {
    define(argument);
  }
  AstVisiter::visit(def);
  decrease();
  define(def.identifier, functions.pop_value());
}

void PassAnalysis::visit(Statement::If& if_) {
  for (auto& branch : if_.branches) {
    AstVisiter::visit(branch.condition);
    increase();
    AstVisiter::visit(branch.statements);
    decrease();
  }
  increase();
  AstVisiter::visit(if_.else_);
  decrease();
}

void PassAnalysis::visit(Statement::Var& var) {
  define(var.identifier);
}

void PassAnalysis::visit(Statement::While& while_) {
  AstVisiter::visit(while_.condition);
  increase();
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

void PassAnalysis::increase() {
  depth++;
}

void PassAnalysis::decrease() {
  depth--;
  while (!variables.empty() && variables.top().depth > depth) {
    variables.pop();
  }
}

template<typename... Args>
void PassAnalysis::define(Args&&... args) {
  variables.emplace(depth, std::forward<Args>(args)...);
}

void PassAnalysis::resolve(const Identifier& identifier) {
  for (auto& variable : sh::reversed(variables)) {
    if (variable.identifier == identifier) {
      if (variable.depth > 0) {
        if (variable.type == Type::Closure && !callee) {
          throw SyntaxError(identifier.location, "cannot dereference function that captures local variables");
        }
        for (auto& function : functions) {
          function = Type::Closure;
        }
      }
      break;
    }
  }
}
