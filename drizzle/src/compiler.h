#pragma once

#include <shell/vector.h>

#include "dzfunction.h"
#include "chunk.h"
#include "interning.h"
#include "opcode.h"
#include "token.h"

class Compiler
{
public:
    Compiler(Interning& interning, Compiler* parent = nullptr);

    void compile(const Tokens& token, DzFunction& function);

private:
    using Labels = shell::Vector<std::size_t, 8>;

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

        Tokens::const_iterator current;
        Tokens::const_iterator previous;
    };

    struct Block
    {
        enum class Type { Block, Conditional, Loop, Function };

        Type type;
        std::string_view identifier;
        Labels breaks;
        Labels continues;
    };

    struct Variable
    {
        std::string_view identifier;
        std::size_t depth;
    };

    struct Upvalue
    {
        std::size_t index;
        bool is_local;
    };

    static const Parser::Rule& rule(Token::Type type);

    template<typename... Bytes>
    void emit(Bytes... bytes);
    void emitReturn();
    void emitConstant(DzValue value, Opcode opcode);
    void emitVariable(std::size_t index, Opcode opcode);
    std::size_t emitJump(Opcode opcode, std::size_t label = 0);
    void patchJump(std::size_t jump);

    void advance();
    bool match(Token::Type type);
    void parse(Precedence precedence);
    void expect(Token::Type type, std::string_view error);
    void expectColon();
    void expectDedent();
    void expectIdentifier();
    void expectIndent();
    void expectNewLine();
    void expectParenLeft();
    void expectParenRight();

    void popLocals(std::size_t depth);
    Labels block(const Block& block, bool increase_scope = true);
    std::size_t resolveVariable(std::string_view identifier);
    std::size_t resolveUpvalue(std::string_view identifier);
    std::size_t addUpvalue(std::size_t index, bool is_local);
    void defineVariable(std::string_view identifier);

    void and_(bool);
    void binary(bool);
    void call(bool);
    void constant(bool);
    void declaration();
    void declarationDef();
    void declarationVar();
    void expression();
    void group(bool);
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
    void statementReturn();
    void statementWhile();
    void unary(bool);
    void variable(bool can_assign);

    Compiler* parent;
    Interning& interning;
    DzFunction* function;
    Parser parser;
    std::vector<Block> scope;
    std::vector<Variable> variables;
    std::vector<Upvalue> upvalues;
};
