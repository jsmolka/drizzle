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
    Call,
    Closure,
    ClosureExt,
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
    LoadUpvalue,
    LoadUpvalueExt,
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
    StoreUpvalue,
    StoreUpvalueExt,
    StoreVariable,
    StoreVariableExt,
    Subtract,
    True,
    LastEnumValue
};

static_assert(std::size_t(Opcode::LastEnumValue) <= std::numeric_limits<u8>::max());
