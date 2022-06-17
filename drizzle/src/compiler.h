#pragma once

#include <sh/stack.h>

#include "astvisiter.h"
#include "dzfunction.h"
#include "opcode.h"

class Gc;

class Compiler final : public AstVisiter {
public:
  Compiler(Gc& gc);

  auto compile(const Stmt& ast) -> DzFunction*;

protected:
  using AstVisiter::visit;

  void visit(Stmt& stmt) final;
  void visit(Statement::Block& block) final;
  void visit(Statement::Break& break_) final;
  void visit(Statement::Class& class_) final;
  void visit(Statement::Continue& continue_) final;
  void visit(Statement::Def& def) final;
  void visit(Statement::ExpressionStatement& expression_statement) final;
  void visit(Statement::For& for_) final;
  void visit(Statement::If& if_) final;
  void visit(Statement::Program& program) final;
  void visit(Statement::Return& return_) final;
  void visit(Statement::Switch& switch_) final;
  void visit(Statement::Var& var) final;
  void visit(Statement::While& while_) final;

  void visit(Expr& expr) final;
  void visit(Expression::Assign& assign) final;
  void visit(Expression::Binary& binary) final;
  void visit(Expression::Call& call) final;
  void visit(Expression::Get& get) final;
  void visit(Expression::In& in) final;
  void visit(Expression::Invoke& invoke) final;
  void visit(Expression::Literal& literal) final;
  void visit(Expression::List& list) final;
  void visit(Expression::Map& map) final;
  void visit(Expression::Range& range) final;
  void visit(Expression::Set& set) final;
  void visit(Expression::SubscriptGet& subscript_get) final;
  void visit(Expression::SubscriptSet& subscript_set) final;
  void visit(Expression::Unary& unary) final;
  void visit(Expression::Variable& variable) final;

private:
  static constexpr auto kJumpSize = 3;

  enum class Type { Main, Function, Init };

  struct Level {
    enum class Type { Block, Branch, Loop, Function };

    Type type;
    std::optional<Identifier> identifier;
    std::vector<std::size_t> breaks;
    std::vector<std::size_t> continues;
  };

  struct Global {
    std::size_t index;
    bool defined;
  };

  struct Variable {
    Identifier identifier;
    std::size_t depth;
  };

  Compiler(Gc& gc, Type type, DzFunction* function, Compiler* parent);

  template<typename... Bytes>
  void emit(Bytes... bytes);
  void emitExt(Opcode opcode, std::size_t value);
  void emitConstant(const DzValue& value);

  auto jump(Opcode opcode, std::optional<std::size_t> label = std::nullopt) -> std::size_t;
  void patch(std::size_t jump);
  void patch(const std::vector<std::size_t>& jumps);

  auto defineLocal(const Identifier& identifier) -> std::size_t;
  void define(const Identifier& identifier);
  auto resolve(const Identifier& identifier) const -> std::optional<std::size_t>;
  auto resolveGlobal(const Identifier& identifier) -> Global&;
  void pop(std::size_t depth);

  template<typename... Args>
  void increaseScope(Args&&... args);
  auto decreaseScope() -> Level;

  Gc& gc;
  Type type;
  Compiler* parent;
  DzFunction* function;
  sh::stack<Level> scope;
  sh::stack<Variable> variables;
  sh::stack<Location> locations;
  tsl::robin_map<Identifier, Global> globals;
};
