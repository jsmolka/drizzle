#pragma once

#include <limits>
#include <sh/fmt.h>

#include "int.h"

enum class Opcode {
  Add,
  AddGeneric,
  AddInt,
  AddFloat,
  BitwiseAnd,
  BitwiseAndGeneric,
  BitwiseAndInt,
  BitwiseAsr,
  BitwiseAsrGeneric,
  BitwiseAsrInt,
  BitwiseComplement,
  BitwiseComplementGeneric,
  BitwiseComplementInt,
  BitwiseLsl,
  BitwiseLslGeneric,
  BitwiseLslInt,
  BitwiseLsr,
  BitwiseLsrGeneric,
  BitwiseLsrInt,
  BitwiseOr,
  BitwiseOrGeneric,
  BitwiseOrInt,
  BitwiseXor,
  BitwiseXorGeneric,
  BitwiseXorInt,
  Call,
  Constant,
  ConstantExt,
  Divide,
  DivideGeneric,
  DivideInt,
  DivideFloat,
  DivideInteger,
  DivideIntegerGeneric,
  DivideIntegerInt,
  DivideIntegerFloat,
  Equal,
  EqualGeneric,
  EqualInt,
  EqualFloat,
  Exit,
  False,
  Get,
  Greater,
  GreaterGeneric,
  GreaterInt,
  GreaterFloat,
  GreaterEqual,
  GreaterEqualGeneric,
  GreaterEqualInt,
  GreaterEqualFloat,
  In,
  Invoke,
  IterInit,
  IterAdvance,
  IterAdvanceExt,
  IterValue,
  IterValueExt,
  Jump,
  JumpFalse,
  JumpFalsePop,
  JumpTrue,
  JumpTruePop,
  Less,
  LessGeneric,
  LessInt,
  LessFloat,
  LessEqual,
  LessEqualGeneric,
  LessEqualInt,
  LessEqualFloat,
  List,
  ListExt,
  Load,
  LoadExt,
  LoadGlobal,
  LoadGlobalExt,
  Map,
  MapExt,
  Modulo,
  ModuloGeneric,
  ModuloInt,
  ModuloFloat,
  Multiply,
  MultiplyGeneric,
  MultiplyInt,
  MultiplyFloat,
  Negate,
  NegateGeneric,
  NegateInt,
  NegateFloat,
  Not,
  NotEqual,
  NotEqualGeneric,
  NotEqualInt,
  NotEqualFloat,
  Null,
  Pop,
  PopMultiple,
  PopMultipleExt,
  Power,
  PowerGeneric,
  PowerInt,
  PowerFloat,
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
  SubtractGeneric,
  SubtractInt,
  SubtractFloat,
  SwitchCase,
  SwitchCaseGeneric,
  SwitchCaseInt,
  True,
  LastEnumValue,
};

static_assert(int(Opcode::LastEnumValue) <= std::numeric_limits<u8>::max());

template<>
struct fmt::formatter<Opcode> : fmt::formatter<std::string_view> {
  static auto repr(const Opcode& opcode) -> std::string_view {
    static_assert(int(Opcode::LastEnumValue) == 121);
    switch (opcode) {
      case Opcode::Add:                      return "Add";
      case Opcode::AddGeneric:               return "AddGeneric";
      case Opcode::AddInt:                   return "AddInt";
      case Opcode::AddFloat:                 return "AddFloat";
      case Opcode::BitwiseAnd:               return "BitwiseAnd";
      case Opcode::BitwiseAndGeneric:        return "BitwiseAndGeneric";
      case Opcode::BitwiseAndInt:            return "BitwiseAndInt";
      case Opcode::BitwiseAsr:               return "BitwiseAsr";
      case Opcode::BitwiseAsrGeneric:        return "BitwiseAsrGeneric";
      case Opcode::BitwiseAsrInt:            return "BitwiseAsrInt";
      case Opcode::BitwiseComplement:        return "BitwiseComplement";
      case Opcode::BitwiseComplementGeneric: return "BitwiseComplementGeneric";
      case Opcode::BitwiseComplementInt:     return "BitwiseComplementInt";
      case Opcode::BitwiseLsl:               return "BitwiseLsl";
      case Opcode::BitwiseLslGeneric:        return "BitwiseLslGeneric";
      case Opcode::BitwiseLslInt:            return "BitwiseLslInt";
      case Opcode::BitwiseLsr:               return "BitwiseLsr";
      case Opcode::BitwiseLsrGeneric:        return "BitwiseLsrGeneric";
      case Opcode::BitwiseLsrInt:            return "BitwiseLsrInt";
      case Opcode::BitwiseOr:                return "BitwiseOr";
      case Opcode::BitwiseOrGeneric:         return "BitwiseOrGeneric";
      case Opcode::BitwiseOrInt:             return "BitwiseOrInt";
      case Opcode::BitwiseXor:               return "BitwiseXor";
      case Opcode::BitwiseXorGeneric:        return "BitwiseXorGeneric";
      case Opcode::BitwiseXorInt:            return "BitwiseXorInt";
      case Opcode::Call:                     return "Call";
      case Opcode::Constant:                 return "Constant";
      case Opcode::ConstantExt:              return "ConstantExt";
      case Opcode::Divide:                   return "Divide";
      case Opcode::DivideGeneric:            return "DivideGeneric";
      case Opcode::DivideInt:                return "DivideInt";
      case Opcode::DivideFloat:              return "DivideFloat";
      case Opcode::DivideInteger:            return "DivideInteger";
      case Opcode::DivideIntegerGeneric:     return "DivideIntegerGeneric";
      case Opcode::DivideIntegerInt:         return "DivideIntegerInt";
      case Opcode::DivideIntegerFloat:       return "DivideIntegerFloat";
      case Opcode::Equal:                    return "Equal";
      case Opcode::EqualGeneric:             return "EqualGeneric";
      case Opcode::EqualInt:                 return "EqualInt";
      case Opcode::EqualFloat:               return "EqualFloat";
      case Opcode::Exit:                     return "Exit";
      case Opcode::False:                    return "False";
      case Opcode::Get:                      return "Get";
      case Opcode::Greater:                  return "Greater";
      case Opcode::GreaterGeneric:           return "GreaterGeneric";
      case Opcode::GreaterInt:               return "GreaterInt";
      case Opcode::GreaterFloat:             return "GreaterFloat";
      case Opcode::GreaterEqual:             return "GreaterEqual";
      case Opcode::GreaterEqualGeneric:      return "GreaterEqualGeneric";
      case Opcode::GreaterEqualInt:          return "GreaterEqualInt";
      case Opcode::GreaterEqualFloat:        return "GreaterEqualFloat";
      case Opcode::In:                       return "In";
      case Opcode::Invoke:                   return "Invoke";
      case Opcode::IterInit:                 return "IterForward";
      case Opcode::IterAdvance:              return "IterAdvance";
      case Opcode::IterAdvanceExt:           return "IterAdvanceExt";
      case Opcode::IterValue:                return "IterValue";
      case Opcode::IterValueExt:             return "IterValueExt";
      case Opcode::Jump:                     return "Jump";
      case Opcode::JumpFalse:                return "JumpFalse";
      case Opcode::JumpFalsePop:             return "JumpFalsePop";
      case Opcode::JumpTrue:                 return "JumpTrue";
      case Opcode::JumpTruePop:              return "JumpTruePop";
      case Opcode::Less:                     return "Less";
      case Opcode::LessGeneric:              return "LessGeneric";
      case Opcode::LessInt:                  return "LessInt";
      case Opcode::LessFloat:                return "LessFloat";
      case Opcode::LessEqual:                return "LessEqual";
      case Opcode::LessEqualGeneric:         return "LessEqualGeneric";
      case Opcode::LessEqualInt:             return "LessEqualInt";
      case Opcode::LessEqualFloat:           return "LessEqualFloat";
      case Opcode::List:                     return "List";
      case Opcode::ListExt:                  return "ListExt";
      case Opcode::Load:                     return "Load";
      case Opcode::LoadExt:                  return "LoadExt";
      case Opcode::LoadGlobal:               return "LoadGlobal";
      case Opcode::LoadGlobalExt:            return "LoadGlobalExt";
      case Opcode::Map:                      return "Map";
      case Opcode::MapExt:                   return "MapExt";
      case Opcode::Modulo:                   return "Modulo";
      case Opcode::ModuloGeneric:            return "ModuloGeneric";
      case Opcode::ModuloInt:                return "ModuloInt";
      case Opcode::ModuloFloat:              return "ModuloFloat";
      case Opcode::Multiply:                 return "Multiply";
      case Opcode::MultiplyGeneric:          return "MultiplyGeneric";
      case Opcode::MultiplyInt:              return "MultiplyInt";
      case Opcode::MultiplyFloat:            return "MultiplyFloat";
      case Opcode::Negate:                   return "Negate";
      case Opcode::NegateGeneric:            return "NegateGeneric";
      case Opcode::NegateInt:                return "NegateInt";
      case Opcode::NegateFloat:              return "NegateFloat";
      case Opcode::Not:                      return "Not";
      case Opcode::NotEqual:                 return "NotEqual";
      case Opcode::NotEqualGeneric:          return "NotEqualGeneric";
      case Opcode::NotEqualInt:              return "NotEqualInt";
      case Opcode::NotEqualFloat:            return "NotEqualFloat";
      case Opcode::Null:                     return "Null";
      case Opcode::Pop:                      return "Pop";
      case Opcode::PopMultiple:              return "PopMultiple";
      case Opcode::PopMultipleExt:           return "PopMultipleExt";
      case Opcode::Power:                    return "Power";
      case Opcode::PowerGeneric:             return "PowerGeneric";
      case Opcode::PowerInt:                 return "PowerInt";
      case Opcode::PowerFloat:               return "PowerFloat";
      case Opcode::Range:                    return "Range";
      case Opcode::Return:                   return "Return";
      case Opcode::Set:                      return "Set";
      case Opcode::Store:                    return "Store";
      case Opcode::StoreExt:                 return "StoreExt";
      case Opcode::StoreGlobal:              return "StoreGlobal";
      case Opcode::StoreGlobalExt:           return "StoreGlobalExt";
      case Opcode::SubscriptGet:             return "SubscriptGet";
      case Opcode::SubscriptSet:             return "SubscriptSet";
      case Opcode::Subtract:                 return "Subtract";
      case Opcode::SubtractGeneric:          return "SubtractGeneric";
      case Opcode::SubtractInt:              return "SubtractInt";
      case Opcode::SubtractFloat:            return "SubtractFloat";
      case Opcode::SwitchCase:               return "SwitchCase";
      case Opcode::SwitchCaseGeneric:        return "SwitchCaseGeneric";
      case Opcode::SwitchCaseInt:            return "SwitchCaseInt";
      case Opcode::True:                     return "True";
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
