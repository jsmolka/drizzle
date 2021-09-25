#pragma once

#include <optional>
#include <shell/buffer.h>

#include "chunk.h"
#include "interning.h"
#include "token.h"

class Compiler
{
public:
    Compiler(Interning& interning);

    void compile(const Tokens& tokens, Chunk& chunk);

private:
    enum Precedence
    {
        kPrecedenceNone,
        kPrecedenceAssignment,  // =
        kPrecedenceOr,          // ||
        kPrecedenceAnd,         // &&
        kPrecedenceBitOr,       // |
        kPrecedenceBitXor,      // ^
        kPrecedenceBitAnd,      // &
        kPrecedenceEquality,    // == !=
        kPrecedenceComparison,  // < > <= >=
        kPrecedenceBitShift,    // << >> >>>
        kPrecedenceTerm,        // + -
        kPrecedenceFactor,      // * / // %
        kPrecedenceUnary,       // ! -
        kPrecedenceCall,        // . ()
        kPrecedencePrimary
    };

    struct Parser
    {
        Token current;
        Token previous;
    };

    using ParseFunction = void(Compiler::*)(bool);

    struct ParseRule
    {
        ParseFunction prefix;
        ParseFunction infix;
        Precedence precedence;
    };

    struct Local
    {
        std::string_view identifier;
        std::size_t depth;
    };

    using Labels = shell::SmallBuffer<std::size_t, 8>;

    struct Block
    {
        enum class Type { Block, Loop, Control };

        Type type;
        std::string_view identifier;
        Labels breaks;
        Labels continues;
    };

    static const ParseRule& rule(Token::Type type);

    template<typename Error, typename... Args>
    void raise(std::string_view message, Args&&... args);

    template<typename... Bytes>
    void emit(Bytes... bytes);
    void emitConstant(DzValue value);
    void emitVariable(std::size_t index, Opcode opcode, Opcode opcode_ext);
    std::size_t emitJump(Opcode opcode, std::size_t label = 0);
    void patchJump(std::size_t jump);

    void advance();
    bool check(Token::Type type) const;
    bool match(Token::Type type);
    void consume(Token::Type type, std::string_view error);
    void consumeColon();
    void consumeDedent();
    void consumeIndent();
    void consumeNewLine();
    void parsePrecedence(Precedence precedence);

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
