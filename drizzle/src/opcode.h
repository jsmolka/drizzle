#pragma once

#include "int.h"

enum class Opcode : u8
{
    Constant,
    ConstantExt,
    Add,
    Subtract,
    Multiply,
    Divide,
    Negate,
    Return
};
