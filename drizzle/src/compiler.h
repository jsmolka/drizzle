#pragma once

#include <shell/buffer.h>

#include "chunk.h"
#include "interning.h"
#include "opcode.h"
#include "token.h"

class Compiler
{
public:
    Compiler(Interning& interning);

    void compile(const Tokens& tokens, Chunk& chunk);

private:
    using Labels = shell::SmallBuffer<std::size_t, 8>;

    enum class Precedence
    {
        None,
        Assignment,
        Or,
        And,
        BitOr,
        BitXor,
        BitAnd,
        Equality,
        Comparison,
        BitShift,
        Term,
        Factor,
        Unary,
        Call,
        Primary
    };

    struct Parser
    {
        struct Rule
        {
            using Parselet = void(Compiler::*)(bool);

            Parselet prefix;
            Parselet infix;
            Precedence precedence;
        };

        Token current;
        Token previous;
    };

    struct Local
    {
        std::string_view identifier;
        std::size_t depth;
    };

    struct Block
    {
        enum class Type { Block, Conditional, Loop };

        Type type;
        std::string_view identifier;
        Labels breaks;
        Labels continues;
    };

    static const Parser::Rule& rule(Token::Type type);

    template<typename... Bytes>
    void emit(Bytes... bytes);
    void emitConstant(DzValue value);
    void emitVariable(std::size_t index, Opcode opcode, Opcode opcode_ext);
    std::size_t emitJump(Opcode opcode, std::size_t label = 0);
    void patchJump(std::size_t jump);

    void advance();
    bool check(Token::Type type) const;
    bool match(Token::Type type);
    void expect(Token::Type type, std::string_view error);
    void expectColon();
    void expectDedent();
    void expectIdentifier();
    void expectIndent();
    void expectNewLine();
    void parse(Precedence precedence);

    void popLocals(std::size_t depth);
    Labels block(const Block& block);

    void and_(bool);
    void binary(bool);
    void constant(bool);
    void declaration();
    void declarationVar();
    void expression();
    void grouping(bool);
    void literal(bool);
    void or_(bool);
    void statement();
    void statementAssert();
    void statementBlock();
    void statementBreak();
    void statementBreakBlock();
    void statementBreakLoop();
    void statementContinue();
    void statementExpression();
    void statementIf();
    void statementNoop();
    void statementPrint();
    void statementWhile();
    void unary(bool);
    void variable(bool can_assign);

    Interning& interning;
    Parser parser;
    Tokens::const_iterator token;
    Chunk* chunk;  // Todo: pass as reference to constructor?
    std::vector<Block> scope;
    std::vector<Local> locals;
};
