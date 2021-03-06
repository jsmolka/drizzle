#include "statement.h"

Statement::Statement(Block block, const Location& location)
  : type(Type::Block), block(std::move(block)), location(location) {}

Statement::Statement(Break break_, const Location& location)
  : type(Type::Break), break_(std::move(break_)), location(location) {}

Statement::Statement(Class class_, const Location& location)
  : type(Type::Class), class_(std::move(class_)), location(location) {}

Statement::Statement(Continue continue_, const Location& location)
  : type(Type::Continue), continue_(std::move(continue_)), location(location) {}

Statement::Statement(Def def, const Location& location)
  : type(Type::Def), def(std::move(def)), location(location) {}

Statement::Statement(ExpressionStatement expression, const Location& location)
  : type(Type::ExpressionStatement), expression_statement(std::move(expression)), location(location) {}

Statement::Statement(For for_, const Location& location)
  : type(Type::For), for_(std::move(for_)), location(location) {}

Statement::Statement(If if_, const Location& location)
  : type(Type::If), if_(std::move(if_)), location(location) {}

Statement::Statement(Noop noop, const Location& location)
  : type(Type::Noop), noop(std::move(noop)), location(location) {}

Statement::Statement(Program program, const Location& location)
  : type(Type::Program), program(std::move(program)), location(location) {}

Statement::Statement(Return return_, const Location& location)
  : type(Type::Return), return_(std::move(return_)), location(location) {}

Statement::Statement(Switch switch_, const Location& location)
  : type(Type::Switch), switch_(std::move(switch_)), location(location) {}

Statement::Statement(Var var, const Location& location)
  : type(Type::Var), var(std::move(var)), location(location) {}

Statement::Statement(While while_, const Location& location)
  : type(Type::While), while_(std::move(while_)), location(location) {}

Statement::~Statement() {
  static_assert(int(Type::LastEnumValue) == 14);

  switch (type) {
    case Type::Block:               std::destroy_at(&block); break;
    case Type::Break:               std::destroy_at(&break_); break;
    case Type::Class:               std::destroy_at(&class_); break;
    case Type::Continue:            std::destroy_at(&continue_); break;
    case Type::Def:                 std::destroy_at(&def); break;
    case Type::ExpressionStatement: std::destroy_at(&expression_statement); break;
    case Type::For:                 std::destroy_at(&for_); break;
    case Type::If:                  std::destroy_at(&if_); break;
    case Type::Noop:                std::destroy_at(&noop); break;
    case Type::Program:             std::destroy_at(&program); break;
    case Type::Return:              std::destroy_at(&return_); break;
    case Type::Switch:              std::destroy_at(&switch_); break;
    case Type::Var:                 std::destroy_at(&var); break;
    case Type::While:               std::destroy_at(&while_); break;
    default:
      SH_UNREACHABLE;
      break;
  }
}
