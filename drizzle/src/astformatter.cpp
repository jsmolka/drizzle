#include "astformatter.h"

auto AstFormatter::format(Stmt& ast) -> std::string {
  walk(ast);
  return string;
}

void AstFormatter::walk(Expr& expr) {
  static_assert(int(Expression::Type::LastEnumValue) == 6);

  writeIndent("{}", expr->type);

  // clang-format off
  switch (expr->type) {
    case Expression::Type::Assign:   write(" {}", expr->assign.identifier); break;
    case Expression::Type::Binary:   write(" {}", expr->binary.type); break;
    case Expression::Type::Literal:  write(" {}", expr->literal); break;
    case Expression::Type::Unary:    write(" {}", expr->unary.type); break;
    case Expression::Type::Variable: write(" {}", expr->variable.identifier); break;
  }

  write("\n");

  indent++;
  AstWalker::walk(expr);
  indent--;
}

void AstFormatter::walk(Stmt& stmt) {
  static_assert(int(Statement::Type::LastEnumValue) == 10);

  writeIndent("{}", stmt->type);

  switch (stmt->type) {
    case Statement::Type::Block:
      write(" {}", stmt->block.identifier);
      break;
    case Statement::Type::Break:
      write(" {}", stmt->break_.identifier);
      break;
    case Statement::Type::Var:
      write(" {}", stmt->var.identifier);
      break;
  }

  write("\n");

  indent++;
  AstWalker::walk(stmt);
  indent--;
}

void AstFormatter::walk(Statement::If& if_) {
  walk(if_.if_.condition);
  walk(if_.if_.statements);

  for (auto& elif : if_.elifs) {
    indent--;
    writeIndent("elif\n");
    indent++;

    walk(elif.condition);
    walk(elif.statements);
  }

  if (!if_.else_.empty()) {
    indent--;
    writeIndent("else\n");
    indent++;

    walk(if_.else_);
  }
}

template <sh::formattable T>
void AstFormatter::write(std::string_view format, const T& value) {
  if constexpr (std::same_as<T, std::string_view>) {
    if (value.empty()) {
      return;
    }
  }
  fmt::format_to(std::back_inserter(string), fmt::runtime(format), value);
}

void AstFormatter::write(std::string_view string) {
  write("{}", string);
}

template <sh::formattable T>
void AstFormatter::writeIndent(std::string_view format, const T& value) {
  if constexpr (std::same_as<T, std::string_view>) {
    if (value.empty()) {
      return;
    }
  }
  fmt::format_to(std::back_inserter(string), "{:<{}}", "", 2 * indent);
  fmt::format_to(std::back_inserter(string), fmt::runtime(format), value);
}

void AstFormatter::writeIndent(std::string_view string) {
  writeIndent("{}", string);
}
