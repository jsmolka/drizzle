#include "compiler.h"

#include <array>
#include <limits>
#include <shell/array.h>
#include <shell/macros.h>

#include "errors.h"

void Compiler::compile(const Tokens& tokens, Chunk& chunk)
{
    this->tokens = tokens;
    this->current_token = &this->tokens[0];
    this->compiling_chunk = &chunk;

    advance();
    expression();

    // Todo: remove once statements are implemented
    while (parser.current.type == Token::Type::NewLine)
        advance();

    consume(Token::Type::Eof, "expected file end");
    endCompiler();
}

const Compiler::ParseRule& Compiler::getRule(Token::Type type)
{
    static constexpr auto kRulesSize = static_cast<std::size_t>(Token::Type::LastEnumValue);
    static constexpr auto kRules = shell::makeArray<ParseRule, kRulesSize>([](std::size_t type) -> ParseRule
    {
        switch (Token::Type(type))
        {
        case Token::Type::ParenLeft: return { &Compiler::grouping, nullptr, kPrecedenceNone };
        case Token::Type::Minus: return { &Compiler::unary, &Compiler::binary, kPrecedenceTerm };
        case Token::Type::Plus: return { nullptr, &Compiler::binary, kPrecedenceTerm };
        case Token::Type::Float: return { &Compiler::constant, nullptr, kPrecedenceTerm };
        case Token::Type::Integer: return { &Compiler::constant, nullptr, kPrecedenceTerm };
        case Token::Type::Bang: return { &Compiler::unary, nullptr, kPrecedenceTerm };

        case Token::Type::Slash:
        case Token::Type::SlashSlash:
        case Token::Type::Star:
        case Token::Type::Percent:
            return { nullptr, &Compiler::binary, kPrecedenceFactor };

        case Token::Type::True:
        case Token::Type::False:
        case Token::Type::Null:
            return { &Compiler::literal, nullptr, kPrecedenceNone };

        case Token::Type::BangEqual:
        case Token::Type::EqualEqual:
            return { nullptr, &Compiler::binary, kPrecedenceEquality };

        case Token::Type::Greater:
        case Token::Type::GreaterEqual:
        case Token::Type::Less:
        case Token::Type::LessEqual:
            return { nullptr, &Compiler::binary, kPrecedenceEquality };
        }
        return { nullptr, nullptr, kPrecedenceNone };
    });
    return kRules[static_cast<std::size_t>(type)];
}

void Compiler::syntaxError(const char* error)
{
    throw SyntaxError(error, parser.previous.lexeme.data());
}

void Compiler::advance()
{
    parser.previous = parser.current;
    parser.current = *current_token;
    current_token++;
}

void Compiler::consume(Token::Type type, const char* error)
{
    if (parser.current.type == type)
    {
        advance();
        return;
    }
    syntaxError(error);
}

void Compiler::expression()
{
    parsePrecedence(kPrecedenceAssignment);
}

void Compiler::grouping()
{
    expression();
    consume(Token::Type::ParenRight, "expected \")\" after expression");
}

void Compiler::constant()
{
    emitConstant(parser.previous.value);
}

void Compiler::unary()
{
    auto prefix = parser.previous.type;

    parsePrecedence(kPrecedenceUnary);

    switch (prefix)
    {
    case Token::Type::Minus:
        emit(Opcode::Negate);
        break;

    case Token::Type::Bang:
        emit(Opcode::Not);
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Compiler::binary()
{
    auto type = parser.previous.type;
    parsePrecedence(Precedence(getRule(type).precedence + 1));

    switch (type)
    {
    case Token::Type::Plus: emit(Opcode::Add); break;
    case Token::Type::Minus: emit(Opcode::Subtract); break;
    case Token::Type::Star: emit(Opcode::Multiply); break;
    case Token::Type::Slash: emit(Opcode::Divide); break;
    case Token::Type::SlashSlash: emit(Opcode::DivideInt); break;
    case Token::Type::BangEqual: emit(Opcode::NotEqual); break;
    case Token::Type::EqualEqual: emit(Opcode::Equal); break;
    case Token::Type::Greater: emit(Opcode::Greater); break;
    case Token::Type::GreaterEqual: emit(Opcode::GreaterEqual); break;
    case Token::Type::Less: emit(Opcode::Less); break;
    case Token::Type::LessEqual: emit(Opcode::LessEqual); break;
    case Token::Type::Percent: emit(Opcode::Modulo); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Compiler::literal()
{
    switch (parser.previous.type)
    {
    case Token::Type::True:
        emit(Opcode::True);
        break;

    case Token::Type::False:
        emit(Opcode::False);
        break;

    case Token::Type::Null:
        emit(Opcode::Null);
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Compiler::parsePrecedence(Precedence precedence)
{
    advance();
    auto prefix = getRule(parser.previous.type).prefix;
    if (!prefix)
        syntaxError("invalid syntax");

    std::invoke(prefix, this);

    while (precedence <= getRule(parser.current.type).precedence)
    {
        advance();
        auto infix = getRule(parser.previous.type).infix;
        if (!infix)
            syntaxError("invalid syntax");

        std::invoke(infix, this);
    }
}

void Compiler::endCompiler()
{
    emitReturn();
}

template<typename... Bytes>
void Compiler::emit(Bytes... bytes)
{
    auto& chunk = currentChunk();

    (chunk.write(bytes, parser.current.line), ...);
}

void Compiler::emitReturn()
{
    emit(Opcode::Return);
}

void Compiler::emitConstant(Value value)
{
    auto& chunk = currentChunk();

    int index = chunk.constants.size();
    if (index <= std::numeric_limits<u8>::max())
        emit(Opcode::Constant, index);
    else if (index <= std::numeric_limits<u16>::max())
        emit(Opcode::ConstantExt, index, index >> 8);
    else
        throw CompilerError("constant limit exceeded");

    chunk.constants.push_back(value);
}

Chunk& Compiler::currentChunk()
{
    return *compiling_chunk;
}
