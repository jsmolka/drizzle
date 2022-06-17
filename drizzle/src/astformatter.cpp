#include "astformatter.h"

#include <sh/ranges.h>

auto AstFormatter::format(const Stmt& ast) -> std::string {
  visit(const_cast<Stmt&>(ast));
  string.pop_back();
  return string;
}

void AstFormatter::visit(Expr& expr) {
  static_assert(int(Expression::Type::LastEnumValue) == 16);

  writeIndent("{}", expr->type);
  switch (expr->type) {
    case Expression::Type::Assign:   write(" {}", expr->assign.identifier); break;
    case Expression::Type::Binary:   write(" {}", expr->binary.type); break;
    case Expression::Type::Get:      write(" {}", expr->get.identifier); break;
    case Expression::Type::Invoke:   write(" {}", expr->invoke.identifier); break;
    case Expression::Type::Literal:  write(" {}", expr->literal.repr()); break;
    case Expression::Type::Set:      write(" {}", expr->set.identifier); break;
    case Expression::Type::Unary:    write(" {}", expr->unary.type); break;
    case Expression::Type::Variable: write(" {}", expr->variable.identifier); break;
  }
  write("\n");

  indent++;
  AstVisiter::visit(expr);
  indent--;
}

void AstFormatter::visit(Stmt& stmt) {
  static_assert(int(Statement::Type::LastEnumValue) == 14);

  writeIndent("{}", stmt->type);
  switch (stmt->type) {
    case Statement::Type::Block:
      if (stmt->block.identifier) {
        write(" {}", *stmt->block.identifier);
      }
      break;
    case Statement::Type::Break:
      if (stmt->break_.identifier) {
        write(" {}", *stmt->break_.identifier);
      }
      break;
    case Statement::Type::Class:
      write(" {}", stmt->class_.identifier);
      break;
    case Statement::Type::Def:
      write(" {}", stmt->def.identifier);
      if (!stmt->def.parameters.empty()) {
        write(" {}", fmt::join(stmt->def.parameters, ", "));
      }
      break;
    case Statement::Type::For:
      write(" {}", stmt->for_.iterator);
      break;
    case Statement::Type::Var:
      write(" {}", stmt->var.identifier);
      break;
  }
  write("\n");

  indent++;
  AstVisiter::visit(stmt);
  indent--;
}

void AstFormatter::visit(Statement::Class& class_) {
  if (class_.methods.empty()) {
    writeIndent("noop\n");
  } else {
    AstVisiter::visit(class_);
  }
}

void AstFormatter::visit(Statement::If& if_) {
  for (const auto [index, branch] : sh::enumerate(if_.branches)) {
    if (index > 0) {
      indent--;
      writeIndent("elif\n");
      indent++;
    }
    visit(branch.condition);
    visit(branch.statements);
  }

  if (if_.else_) {
    indent--;
    writeIndent("else\n");
    indent++;
    visit(*if_.else_);
  }
}

void AstFormatter::visit(Statement::Switch& switch_) {
  visit(switch_.value);
  for (auto& case_ : switch_.cases) {
    writeIndent("case\n");
    indent++;
    visit(case_.value);
    visit(case_.statements);
    indent--;
  }

  if (switch_.default_) {
    writeIndent("default\n");
    indent++;
    visit(*switch_.default_);
    indent--;
  }
}

template<typename... Args>
void AstFormatter::write(std::string_view format, Args&&... args) {
  fmt::format_to(std::back_inserter(string), fmt::runtime(format), std::forward<Args>(args)...);
}

template<typename... Args>
void AstFormatter::writeIndent(std::string_view format, Args&&... args) {
  fmt::format_to(std::back_inserter(string), "{:<{}}", "", 2 * indent);
  fmt::format_to(std::back_inserter(string), fmt::runtime(format), std::forward<Args>(args)...);
}
