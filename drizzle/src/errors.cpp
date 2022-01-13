#include "errors.h"

const char* Error::what() const noexcept {
  return message.c_str();
}

const char* SyntaxError::name() const noexcept {
  return "SyntaxError";
}

const char* CompilerError::name() const noexcept {
  return "CompilerError";
}

const char* RuntimeError::name() const noexcept {
  return "RuntimeError";
}
