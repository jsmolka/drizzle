#pragma once

#include <sh/fmt.h>

#include "astvisiter.h"

class AstFormatter final : public AstVisiter {
public:
  auto format(const Stmt& ast) -> std::string;

protected:
  using AstVisiter::visit;

  void visit(Expr& expr) final;
  void visit(Stmt& stmt) final;
  void visit(Statement::If& if_) final;

private:
  template<sh::formattable T>
  void write(std::string_view format, const T& value);
  void write(std::string_view string);

  template<sh::formattable T>
  void writeIndent(std::string_view format, const T& value);
  void writeIndent(std::string_view string);

  std::string string;
  std::size_t indent = 0;
};

template<>
struct fmt::formatter<Stmt> : fmt::formatter<std::string> {
  template<typename FormatContext>
  auto format(const Stmt& ast, FormatContext& ctx) const {
    return fmt::formatter<std::string>::format(AstFormatter().format(ast), ctx);
  }
};
