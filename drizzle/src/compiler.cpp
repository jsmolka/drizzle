#include "compiler.h"

#include <limits>
#include <shell/array.h>
#include <shell/macros.h>

#include "errors.h"

void Compiler::compile(const Tokens& tokens, Chunk& chunk)
{
    this->token = tokens.begin();
    this->chunk = &chunk;

    advance();
    expression();

    // Todo: remove once statements are implemented
    while (parser.current.type == Token::Type::NewLine)
        advance();

    consume(Token::Type::Eof, "expected file end");
    emit(Opcode::Exit);
}

const Compiler::ParseRule& Compiler::rule(Token::Type type)
{
    static constexpr auto kRulesSize = static_cast<std::size_t>(Token::Type::LastEnumValue);
    static constexpr auto kRules = shell::makeArray<ParseRule, kRulesSize>([](std::size_t type) -> ParseRule
    {
        switch (Token::Type(type))
        {
        case Token::Type::Bang:         return { &Compiler::unary,    nullptr,           kPrecedenceTerm     };
        case Token::Type::BangEqual:    return { nullptr,             &Compiler::binary, kPrecedenceEquality };
        case Token::Type::EqualEqual:   return { nullptr,             &Compiler::binary, kPrecedenceEquality };
        case Token::Type::False:        return { &Compiler::literal,  nullptr,           kPrecedenceNone     };
        case Token::Type::Float:        return { &Compiler::constant, nullptr,           kPrecedenceTerm     };
        case Token::Type::Greater:      return { nullptr,             &Compiler::binary, kPrecedenceEquality };
        case Token::Type::GreaterEqual: return { nullptr,             &Compiler::binary, kPrecedenceEquality };
        case Token::Type::Integer:      return { &Compiler::constant, nullptr,           kPrecedenceTerm     };
        case Token::Type::Less:         return { nullptr,             &Compiler::binary, kPrecedenceEquality };
        case Token::Type::LessEqual:    return { nullptr,             &Compiler::binary, kPrecedenceEquality };
        case Token::Type::Minus:        return { &Compiler::unary,    &Compiler::binary, kPrecedenceTerm     };
        case Token::Type::Null:         return { &Compiler::literal,  nullptr,           kPrecedenceNone     };
        case Token::Type::ParenLeft:    return { &Compiler::grouping, nullptr,           kPrecedenceNone     };
        case Token::Type::Percent:      return { nullptr,             &Compiler::binary, kPrecedenceFactor   };
        case Token::Type::Plus:         return { nullptr,             &Compiler::binary, kPrecedenceTerm     };
        case Token::Type::Slash:        return { nullptr,             &Compiler::binary, kPrecedenceFactor   };
        case Token::Type::SlashSlash:   return { nullptr,             &Compiler::binary, kPrecedenceFactor   };
        case Token::Type::Star:         return { nullptr,             &Compiler::binary, kPrecedenceFactor   };
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
    (chunk->write(bytes, parser.current.line), ...);
}

void Compiler::emitConstant(Value value)
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
    case Token::Type::BangEqual:    emit(Opcode::NotEqual); break;
    case Token::Type::EqualEqual:   emit(Opcode::Equal); break;
    case Token::Type::Greater:      emit(Opcode::Greater); break;
    case Token::Type::GreaterEqual: emit(Opcode::GreaterEqual); break;
    case Token::Type::Less:         emit(Opcode::Less); break;
    case Token::Type::LessEqual:    emit(Opcode::LessEqual); break;
    case Token::Type::Minus:        emit(Opcode::Subtract); break;
    case Token::Type::Plus:         emit(Opcode::Add); break;
    case Token::Type::Percent:      emit(Opcode::Modulo); break;
    case Token::Type::Slash:        emit(Opcode::Divide); break;
    case Token::Type::SlashSlash:   emit(Opcode::DivideInt); break;
    case Token::Type::Star:         emit(Opcode::Multiply); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Compiler::constant()
{
    emitConstant(parser.previous.value);
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

void Compiler::unary()
{
    auto token = parser.previous.type;
    parsePrecedence(kPrecedenceUnary);

    switch (token)
    {
    case Token::Type::Bang:  emit(Opcode::Not); break;
    case Token::Type::Minus: emit(Opcode::Negate); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}
