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
  False,
  Greater,
  GreaterEqual,
  Jump,
  JumpFalse,
  JumpFalsePop,
  JumpTrue,
  Less,
  LessEqual,
  Load,
  LoadExt,
  LoadCapture,
  LoadCaptureExt,
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
  Store,
  StoreExt,
  StoreCapture,
  StoreCaptureExt,
  Subtract,
  True,
  LastEnumValue,
};

static_assert(int(Opcode::LastEnumValue) <= std::numeric_limits<u8>::max());

template<>
struct fmt::formatter<Opcode> : fmt::formatter<std::string_view> {
  template<typename FormatContext>
  auto format(const Opcode& opcode, FormatContext& ctx) const {
    auto repr = [](const Opcode& opcode) {
      static_assert(int(Opcode::LastEnumValue) == 44);
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
        case Opcode::False:             return "False";
        case Opcode::Greater:           return "Greater";
        case Opcode::GreaterEqual:      return "GreaterEqual";
        case Opcode::Jump:              return "Jump";
        case Opcode::JumpFalse:         return "JumpFalse";
        case Opcode::JumpFalsePop:      return "JumpFalsePop";
        case Opcode::JumpTrue:          return "JumpTrue";
        case Opcode::Less:              return "Less";
        case Opcode::LessEqual:         return "LessEqual";
        case Opcode::Load:              return "Load";
        case Opcode::LoadExt:           return "LoadExt";
        case Opcode::LoadCapture:       return "LoadCapture";
        case Opcode::LoadCaptureExt:    return "LoadCaptureExt";
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
        case Opcode::Store:             return "Store";
        case Opcode::StoreExt:          return "StoreExt";
        case Opcode::StoreCapture:      return "StoreCapture";
        case Opcode::StoreCaptureExt:   return "StoreCaptureExt";
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
