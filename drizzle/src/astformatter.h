#pragma once

#include "astwalker.h"
#include "format.h"

class AstFormatter final : public AstWalker {
 public:
  auto format(Stmt& ast) -> std::string;

 protected:
  using AstWalker::walk;

  void walk(Expr& expr) final;
  void walk(Stmt& stmt) final;
  void walk(Statement::If& if_) final;

 private:
  template <sh::formattable T>
  void write(std::string_view format, const T& value);
  void write(std::string_view format);

  template <sh::formattable T>
  void writeIndent(std::string_view format, const T& value);
  void writeIndent(std::string_view format);

  std::string buffer;
  std::size_t indentation;
};
