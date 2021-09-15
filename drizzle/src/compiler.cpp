#include "compiler.h"

#include <array>
#include <cerrno>
#include <cstdlib>
#include <limits>
#include <shell/array.h>
#include <shell/macros.h>

#include "error.h"

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
        case Token::Type::Slash: return { nullptr, &Compiler::binary, kPrecedenceFactor };
        case Token::Type::Star: return { nullptr, &Compiler::binary, kPrecedenceFactor };
        case Token::Type::Float: return { &Compiler::number, nullptr, kPrecedenceTerm };
        }
        return { nullptr, nullptr, kPrecedenceNone };
    });
    return kRules[static_cast<std::size_t>(type)];
}

void Compiler::syntaxError(const char* error)
{
    throw SyntaxError(error, parser.current.lexeme.data());
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

void Compiler::unary()
{
    auto prefix = parser.previous.type;

    parsePrecedence(kPrecedenceUnary);

    switch (prefix)
    {
    case Token::Type::Minus:
        emit(Opcode::Negate);
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
    case Token::Type::Plus:
        emit(Opcode::Add);
        break;

    case Token::Type::Minus:
        emit(Opcode::Subtract);
        break;

    case Token::Type::Star:
        emit(Opcode::Multiply);
        break;

    case Token::Type::Slash:
        emit(Opcode::Divide);
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Compiler::number()
{
    // Todo: handle integer
    double value = std::strtod(parser.previous.lexeme.data(), nullptr);
    if (errno == ERANGE)
    {
        // Todo: throw RuntimeError
        errno = 0;
    }
    emitConstant(value);
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

    (chunk.write(bytes), ...);
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
    {
        chunk.write(Opcode::Constant);
        chunk.write(index);
    }
    else if (index <= std::numeric_limits<u16>::max())
    {
        chunk.write(Opcode::ConstantExt);
        chunk.write(index);
        chunk.write(index >> 8);
    }
    else
    {
        throw Error("constant limit exceeded");
    }
    chunk.constants.push_back(value);
}

Chunk& Compiler::currentChunk()
{
    return *compiling_chunk;
}
