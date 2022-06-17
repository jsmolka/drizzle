#pragma once

#include "astvisiter.h"

class AstFormatter final : public AstVisiter {
public:
  auto format(const Stmt& ast) -> std::string;

protected:
  using AstVisiter::visit;

  void visit(Expr& expr) final;
  void visit(Stmt& stmt) final;
  void visit(Statement::Class& class_) final;
  void visit(Statement::If& if_) final;
  void visit(Statement::Switch& switch_) final;

private:
  template<typename... Args>
  void write(std::string_view format, Args&&... args);
  template<typename... Args>
  void writeIndent(std::string_view format, Args&&... args);

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
