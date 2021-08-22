#include "compiler.h"

#include <cerrno>
#include <cstdlib>
#include <limits>
#include <shell/macros.h>

#include "error.h"

void Compiler::compile(const Tokens& tokens, Chunk& chunk)
{
    this->tokens = tokens;
    this->current_token = &this->tokens[0];
    this->compiling_chunk = &chunk;

    advance();
    expression();
    consume(Token::Type::Eof, "expected file end");
    endCompiler();
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

    throw SyntaxError(parser.current.lexeme.data(), error);
}

void Compiler::expression()
{

}

void Compiler::grouping()
{
    expression();
    consume(Token::Type::ParenRight, "expected \")\" after expression");
}

void Compiler::unary()
{
    auto prefix = parser.previous.type;

    expression();

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
