#include "compiler.h"

#include <limits>
#include <shell/array.h>
#include <shell/macros.h>

#include "dzstring.h"
#include "errors.h"

void Compiler::compile(const Tokens& tokens, Chunk& chunk)
{
    this->token = tokens.begin();
    this->chunk = &chunk;

    advance();
    
    while (!match(Token::Type::Eof))
        declaration();

    emit(Opcode::Exit);
}

const Compiler::ParseRule& Compiler::rule(Token::Type type)
{
    static constexpr auto kRulesSize = static_cast<std::size_t>(Token::Type::LastEnumValue);
    static constexpr auto kRules = shell::makeArray<ParseRule, kRulesSize>([](std::size_t type) -> ParseRule
    {
        switch (Token::Type(type))
        {
        case Token::Type::And:          return { nullptr,             &Compiler::binary, kPrecedenceBitAnd   };
        case Token::Type::Bang:         return { &Compiler::unary,    nullptr,           kPrecedenceTerm     };
        case Token::Type::BangEqual:    return { nullptr,             &Compiler::binary, kPrecedenceEquality };
        case Token::Type::Caret:        return { nullptr,             &Compiler::binary, kPrecedenceBitXor   };
        case Token::Type::Equal2:       return { nullptr,             &Compiler::binary, kPrecedenceEquality };
        case Token::Type::False:        return { &Compiler::literal,  nullptr,           kPrecedenceNone     };
        case Token::Type::Float:        return { &Compiler::constant, nullptr,           kPrecedenceTerm     };
        case Token::Type::Greater:      return { nullptr,             &Compiler::binary, kPrecedenceEquality };
        case Token::Type::Greater2:     return { nullptr,             &Compiler::binary, kPrecedenceBitShift };
        case Token::Type::Greater3:     return { nullptr,             &Compiler::binary, kPrecedenceBitShift };
        case Token::Type::GreaterEqual: return { nullptr,             &Compiler::binary, kPrecedenceEquality };
        case Token::Type::Integer:      return { &Compiler::constant, nullptr,           kPrecedenceTerm     };
        case Token::Type::Less:         return { nullptr,             &Compiler::binary, kPrecedenceEquality };
        case Token::Type::Less2:        return { nullptr,             &Compiler::binary, kPrecedenceBitShift };
        case Token::Type::LessEqual:    return { nullptr,             &Compiler::binary, kPrecedenceEquality };
        case Token::Type::Minus:        return { &Compiler::unary,    &Compiler::binary, kPrecedenceTerm     };
        case Token::Type::Null:         return { &Compiler::literal,  nullptr,           kPrecedenceNone     };
        case Token::Type::ParenLeft:    return { &Compiler::grouping, nullptr,           kPrecedenceNone     };
        case Token::Type::Percent:      return { nullptr,             &Compiler::binary, kPrecedenceFactor   };
        case Token::Type::Pipe:         return { nullptr,             &Compiler::binary, kPrecedenceBitOr    };
        case Token::Type::Plus:         return { nullptr,             &Compiler::binary, kPrecedenceTerm     };
        case Token::Type::Slash:        return { nullptr,             &Compiler::binary, kPrecedenceFactor   };
        case Token::Type::Slash2:       return { nullptr,             &Compiler::binary, kPrecedenceFactor   };
        case Token::Type::Star:         return { nullptr,             &Compiler::binary, kPrecedenceFactor   };
        case Token::Type::Star2:        return { nullptr,             &Compiler::binary, kPrecedenceFactor   };
        case Token::Type::String:       return { &Compiler::constant, nullptr,           kPrecedenceNone     };
        case Token::Type::Tilde:        return { &Compiler::unary,    nullptr,           kPrecedenceUnary    };
        case Token::Type::True:         return { &Compiler::literal,  nullptr,           kPrecedenceNone     };
        }
        return { nullptr, nullptr, kPrecedenceNone };
    });
    return kRules[static_cast<std::size_t>(type)];
}

template<typename Error, typename ...Args>
void Compiler::raise(std::string_view message, Args&& ...args)
{
    static_assert(std::is_base_of_v<LocationError, Error>);

    throw SyntaxError(parser.previous.lexeme.data(), message, std::forward<Args>(args)...);
}

template<typename... Bytes>
void Compiler::emit(Bytes... bytes)
{
    (chunk->write(bytes, parser.previous.line), ...);
}

void Compiler::emitConstant(DzValue value)
{
    int index = chunk->constants.size();
    if (index <= std::numeric_limits<u8>::max())
        emit(Opcode::Constant, index);
    else if (index <= std::numeric_limits<u16>::max())
        emit(Opcode::ConstantExt, index, index >> 8);
    else
        throw CompilerError("constant limit exceeded");

    chunk->constants.push_back(value);
}

void Compiler::advance()
{
    parser.previous = parser.current;
    parser.current  = *token++;
}

bool Compiler::check(Token::Type type) const
{
    return parser.current.type == type;
}

bool Compiler::match(Token::Type type)
{
    if (check(type))
    {
        advance();
        return true;
    }
    return false;
}

void Compiler::consume(Token::Type type, std::string_view error)
{
    if (parser.current.type != type)
        raise<SyntaxError>(error);

    advance();
}

void Compiler::parsePrecedence(Precedence precedence)
{
    advance();
    auto prefix = rule(parser.previous.type).prefix;
    if (!prefix)
        raise<SyntaxError>("invalid syntax");

    std::invoke(prefix, this);

    while (precedence <= rule(parser.current.type).precedence)
    {
        advance();
        auto infix = rule(parser.previous.type).infix;
        if (!infix)
            raise<SyntaxError>("invalid syntax");

        std::invoke(infix, this);
    }
}

void Compiler::binary()
{
    auto token = parser.previous.type;
    parsePrecedence(Precedence(rule(token).precedence + 1));

    switch (token)
    {
    case Token::Type::And:          emit(Opcode::BitAnd); break;
    case Token::Type::BangEqual:    emit(Opcode::NotEqual); break;
    case Token::Type::Caret:        emit(Opcode::BitXor); break;
    case Token::Type::Equal2:       emit(Opcode::Equal); break;
    case Token::Type::Greater:      emit(Opcode::Greater); break;
    case Token::Type::Greater2:     emit(Opcode::BitAsr); break;
    case Token::Type::Greater3:     emit(Opcode::BitLsr); break;
    case Token::Type::GreaterEqual: emit(Opcode::GreaterEqual); break;
    case Token::Type::Less:         emit(Opcode::Less); break;
    case Token::Type::Less2:        emit(Opcode::BitLsl); break;
    case Token::Type::LessEqual:    emit(Opcode::LessEqual); break;
    case Token::Type::Minus:        emit(Opcode::Subtract); break;
    case Token::Type::Plus:         emit(Opcode::Add); break;
    case Token::Type::Percent:      emit(Opcode::Modulo); break;
    case Token::Type::Pipe:         emit(Opcode::BitOr); break;
    case Token::Type::Slash:        emit(Opcode::Divide); break;
    case Token::Type::Slash2:       emit(Opcode::DivideInt); break;
    case Token::Type::Star:         emit(Opcode::Multiply); break;
    case Token::Type::Star2:        emit(Opcode::Power); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Compiler::constant()
{
    emitConstant(parser.previous.value);
}

void Compiler::declaration()
{
    statement();
}

void Compiler::expression()
{
    parsePrecedence(kPrecedenceAssignment);
}

void Compiler::grouping()
{
    expression();
    consume(Token::Type::ParenRight, "expected ')'");
}

void Compiler::literal()
{
    switch (parser.previous.type)
    {
    case Token::Type::False: emit(Opcode::False); break;
    case Token::Type::Null:  emit(Opcode::Null); break;
    case Token::Type::True:  emit(Opcode::True); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Compiler::statement()
{
    if (match(Token::Type::Assert))
        statementAssert();
    else if (match(Token::Type::Print))
        statementPrint();
}

void Compiler::statementAssert()
{
    expression();
    consume(Token::Type::NewLine, "expected new line");
    emit(Opcode::Assert);
}

void Compiler::statementExpression()
{
    expression();
    consume(Token::Type::NewLine, "expected new line");
    emit(Opcode::Discard);
}

void Compiler::statementPrint()
{
    expression();
    consume(Token::Type::NewLine, "expected new line");
    emit(Opcode::Print);
}

void Compiler::unary()
{
    auto token = parser.previous.type;
    parsePrecedence(kPrecedenceUnary);

    switch (token)
    {
    case Token::Type::Bang:  emit(Opcode::Not); break;
    case Token::Type::Minus: emit(Opcode::Negate); break;
    case Token::Type::Tilde: emit(Opcode::BitComplement); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}
