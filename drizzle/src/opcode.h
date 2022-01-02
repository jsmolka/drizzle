#pragma once

#include <limits>

#include "int.h"

enum class Opcode {
  Add,
  BitwiseAnd,
  BitwiseAsr,
  BitwiseComplement,
  BitwiseLsl,
  BitwiseLsr,
  BitwiseOr,
  BitwiseXor,
  Constant,
  ConstantExt,
  Divide,
  DivideInt,
  Equal,
  False,
  Greater,
  GreaterEqual,
  Jump,
  JumpFalse,
  JumpFalsePop,
  JumpTrue,
  Less,
  LessEqual,
  LoadVariable,
  LoadVariableExt,
  Modulo,
  Multiply,
  Negate,
  Not,
  NotEqual,
  Null,
  Pop,
  PopMultiple,
  PopMultipleExt,
  Power,
  Print,
  Return,
  StoreVariable,
  StoreVariableExt,
  Subtract,
  True,
  LastEnumValue,
};

static_assert(int(Opcode::LastEnumValue) <= std::numeric_limits<u8>::max());
