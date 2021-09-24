#pragma once

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

    static const ParseRule& rule(Token::Type type);

    template<typename Error, typename... Args>
    void raise(std::string_view message, Args&&... args);

    template<typename... Bytes>
    void emit(Bytes... bytes);
    void emitConstant(DzValue value);
    void emitVariable(std::size_t index, Opcode opcode, Opcode opcode_ext);
    std::size_t emitJump(Opcode opcode);
    void patchJump(std::size_t offset);

    void advance();
    bool check(Token::Type type) const;
    bool match(Token::Type type);
    void consume(Token::Type type, std::string_view error);
    void consumeColon();
    void consumeDedent();
    void consumeIndent();
    void consumeNewLine();

    void beginScope();
    void endScope();

    void parsePrecedence(Precedence precedence);

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
    void statementExpression();
    void statementIf();
    void statementPrint();
    void unary(bool);
    void variable(bool can_assign);

    Parser parser;
    Tokens::const_iterator token;
    Chunk* chunk;
    Interning& interning;
    std::size_t scope_depth;
    std::vector<Local> locals;
};
