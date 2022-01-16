#pragma once

#include "astwalker.h"

#include <sh/fmt.h>

class AstFormatter final : public AstWalker {
public:
  auto format(const Stmt& ast) -> std::string;

protected:
  using AstWalker::walk;

  void walk(Expr& expr) final;
  void walk(Stmt& stmt) final;
  void walk(Statement::If& if_) final;

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
