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
  In,
  Invoke,
  IterInit,
  IterAdvance,
  IterAdvanceExt,
  IterCurrent,
  IterCurrentExt,
  Jump,
  JumpFalse,
  JumpFalsePop,
  JumpTrue,
  JumpTruePop,
  Less,
  LessEqual,
  List,
  ListExt,
  Load,
  LoadExt,
  LoadGlobal,
  LoadGlobalExt,
  Map,
  MapExt,
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
  Range,
  Return,
  Set,
  Store,
  StoreExt,
  StoreGlobal,
  StoreGlobalExt,
  SubscriptGet,
  SubscriptSet,
  Subtract,
  True,
  LastEnumValue,
};

static_assert(int(Opcode::LastEnumValue) <= std::numeric_limits<u8>::max());

template<>
struct fmt::formatter<Opcode> : fmt::formatter<std::string_view> {
  static auto repr(const Opcode& opcode) -> std::string_view {
    static_assert(int(Opcode::LastEnumValue) == 62);
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
      case Opcode::In:                return "In";
      case Opcode::Invoke:            return "Invoke";
      case Opcode::IterInit:          return "IterForward";
      case Opcode::IterAdvance:       return "IterAdvance";
      case Opcode::IterAdvanceExt:    return "IterAdvanceExt";
      case Opcode::IterCurrent:       return "IterCurrent";
      case Opcode::IterCurrentExt:    return "IterCurrentExt";
      case Opcode::Jump:              return "Jump";
      case Opcode::JumpFalse:         return "JumpFalse";
      case Opcode::JumpFalsePop:      return "JumpFalsePop";
      case Opcode::JumpTrue:          return "JumpTrue";
      case Opcode::JumpTruePop:       return "JumpTruePop";
      case Opcode::Less:              return "Less";
      case Opcode::LessEqual:         return "LessEqual";
      case Opcode::List:              return "List";
      case Opcode::ListExt:           return "ListExt";
      case Opcode::Load:              return "Load";
      case Opcode::LoadExt:           return "LoadExt";
      case Opcode::LoadGlobal:        return "LoadGlobal";
      case Opcode::LoadGlobalExt:     return "LoadGlobalExt";
      case Opcode::Map:               return "Map";
      case Opcode::MapExt:            return "MapExt";
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
      case Opcode::Range:             return "Range";
      case Opcode::Return:            return "Return";
      case Opcode::Set:               return "Set";
      case Opcode::Store:             return "Store";
      case Opcode::StoreExt:          return "StoreExt";
      case Opcode::StoreGlobal:       return "StoreGlobal";
      case Opcode::StoreGlobalExt:    return "StoreGlobalExt";
      case Opcode::SubscriptGet:      return "SubscriptGet";
      case Opcode::SubscriptSet:      return "SubscriptSet";
      case Opcode::Subtract:          return "Subtract";
      case Opcode::True:              return "True";
      default:
        SH_UNREACHABLE;
        return "unreachable";
    }
  }

  template<typename FormatContext>
  auto format(const Opcode& opcode, FormatContext& ctx) const {
    return fmt::formatter<std::string_view>::format(repr(opcode), ctx);
  }
};
