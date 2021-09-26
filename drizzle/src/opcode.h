#pragma once

#include <limits>

#include "int.h"

enum class Opcode
{
    Add,
    Assert,
    BitAnd,
    BitAsr,
    BitComplement,
    BitLsl,
    BitLsr,
    BitOr,
    BitXor,
    Constant,
    ConstantExt,
    Divide,
    DivideInt,
    Equal,
    Exit,
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
    StoreVariable,
    StoreVariableExt,
    Subtract,
    True,
    LastEnumValue
};

static_assert(std::size_t(Opcode::LastEnumValue) <= std::numeric_limits<u8>::max());
