#include "statement.h"

#include <sh/utility.h>

Statement::Statement(Block block, const Location& location)
  : type(Type::Block), block(std::move(block)), location(location) {}

Statement::Statement(Break break_, const Location& location)
  : type(Type::Break), break_(std::move(break_)), location(location) {}

Statement::Statement(Continue continue_, const Location& location)
  : type(Type::Continue), continue_(std::move(continue_)), location(location) {}

Statement::Statement(ExpressionStatement expression, const Location& location)
  : type(Type::ExpressionStatement), expression_statement(std::move(expression)), location(location) {}

Statement::Statement(If if_, const Location& location)
  : type(Type::If), if_(std::move(if_)), location(location) {}

Statement::Statement(Noop noop, const Location& location)
  : type(Type::Noop), noop(std::move(noop)), location(location) {}

Statement::Statement(Print print, const Location& location)
  : type(Type::Print), print(std::move(print)), location(location) {}

Statement::Statement(Program program, const Location& location)
  : type(Type::Program), program(std::move(program)), location(location) {}

Statement::Statement(Var var, const Location& location)
  : type(Type::Var), var(std::move(var)), location(location) {}

Statement::Statement(While while_, const Location& location)
  : type(Type::While), while_(std::move(while_)), location(location) {}

Statement::~Statement() {
  switch (type) {
    case Type::Block:               std::destroy_at(&block); break;
    case Type::Break:               std::destroy_at(&break_); break;
    case Type::Continue:            std::destroy_at(&continue_); break;
    case Type::ExpressionStatement: std::destroy_at(&expression_statement); break;
    case Type::If:                  std::destroy_at(&if_); break;
    case Type::Noop:                std::destroy_at(&noop); break;
    case Type::Print:               std::destroy_at(&print); break;
    case Type::Program:             std::destroy_at(&program); break;
    case Type::Var:                 std::destroy_at(&var); break;
    case Type::While:               std::destroy_at(&while_); break;
    default:
      SH_UNREACHABLE;
      break;
  }
}
