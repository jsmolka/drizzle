#pragma once

#include <limits>

#include <sh/fmt.h>

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
  Call,
  Constant,
  ConstantExt,
  Divide,
  DivideInt,
  Equal,
  Exit,
  False,
  Get,
  Greater,
  GreaterEqual,
  Invoke,
  Jump,
  JumpFalse,
  JumpFalsePop,
  JumpTrue,
  Less,
  LessEqual,
  Load,
  LoadExt,
  LoadGlobal,
  LoadGlobalExt,
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
  Return,
  Set,
  Store,
  StoreExt,
  StoreGlobal,
  StoreGlobalExt,
  Subtract,
  True,
  LastEnumValue,
};

static_assert(int(Opcode::LastEnumValue) <= std::numeric_limits<u8>::max());

template<>
struct fmt::formatter<Opcode> : fmt::formatter<std::string_view> {
  static_assert(int(Opcode::LastEnumValue) == 48);
  template<typename FormatContext>
  auto format(const Opcode& opcode, FormatContext& ctx) const {
    auto repr = [](const Opcode& opcode) {
      switch (opcode) {
        case Opcode::Add:               return "Add";
        case Opcode::BitwiseAnd:        return "BitwiseAnd";
        case Opcode::BitwiseAsr:        return "BitwiseAsr";
        case Opcode::BitwiseComplement: return "BitwiseComplement";
        case Opcode::BitwiseLsl:        return "BitwiseLsl";
        case Opcode::BitwiseLsr:        return "BitwiseLsr";
        case Opcode::BitwiseOr:         return "BitwiseOr";
        case Opcode::BitwiseXor:        return "BitwiseXor";
        case Opcode::Call:              return "Call";
        case Opcode::Constant:          return "Constant";
        case Opcode::ConstantExt:       return "ConstantExt";
        case Opcode::Divide:            return "Divide";
        case Opcode::DivideInt:         return "DivideInt";
        case Opcode::Equal:             return "Equal";
        case Opcode::Exit:              return "Exit";
        case Opcode::False:             return "False";
        case Opcode::Get:               return "Get";
        case Opcode::Greater:           return "Greater";
        case Opcode::GreaterEqual:      return "GreaterEqual";
        case Opcode::Invoke:            return "Invoke";
        case Opcode::Jump:              return "Jump";
        case Opcode::JumpFalse:         return "JumpFalse";
        case Opcode::JumpFalsePop:      return "JumpFalsePop";
        case Opcode::JumpTrue:          return "JumpTrue";
        case Opcode::Less:              return "Less";
        case Opcode::LessEqual:         return "LessEqual";
        case Opcode::Load:              return "Load";
        case Opcode::LoadExt:           return "LoadExt";
        case Opcode::LoadGlobal:        return "LoadGlobal";
        case Opcode::LoadGlobalExt:     return "LoadGlobalExt";
        case Opcode::Modulo:            return "Modulo";
        case Opcode::Multiply:          return "Multiply";
        case Opcode::Negate:            return "Negate";
        case Opcode::Not:               return "Not";
        case Opcode::NotEqual:          return "NotEqual";
        case Opcode::Null:              return "Null";
        case Opcode::Pop:               return "Pop";
        case Opcode::PopMultiple:       return "PopMultiple";
        case Opcode::PopMultipleExt:    return "PopMultipleExt";
        case Opcode::Power:             return "Power";
        case Opcode::Return:            return "Return";
        case Opcode::Set:               return "Set";
        case Opcode::Store:             return "Store";
        case Opcode::StoreExt:          return "StoreExt";
        case Opcode::StoreGlobal:       return "StoreGlobal";
        case Opcode::StoreGlobalExt:    return "StoreGlobalExt";
        case Opcode::Subtract:          return "Subtract";
        case Opcode::True:              return "True";
        default:
          SH_UNREACHABLE;
          return "unreachable";
      }
    };
    return fmt::formatter<std::string_view>::format(repr(opcode), ctx);
  }
};
