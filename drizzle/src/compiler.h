#pragma once

#include "chunk.h"
#include "opcode.h"
#include "statement.h"
#include "stringpool.h"

class Compiler
{
public:
    Compiler(StringPool& pool);

    void compile(const Stmt& ast, Chunk& chunk);

private:
    struct Block
    {
        enum class Type { Block, Conditional, Loop, Function };

        Type type;
        std::string_view identifier;
    };

    struct Variable
    {
        std::string_view identifier;
        std::size_t depth;
    };

    template<typename... Bytes>
    void emit(Bytes... bytes);
    void emitConstant(DzValue value);
    void emitVariable(std::size_t index, Opcode opcode);

    void defineVariable(std::string_view identifier);
    Variable& resolveVariable(std::string_view identifier);
    void popVariables(std::size_t depth);

    void compile(const Stmt& stmt);
    void compile(const Statement::Block& block);
    void compile(const Statement::ExpressionStatement& expression_statement);
    void compile(const Statement::Print& print);
    void compile(const Statement::Program& program);
    void compile(const Statement::Var& var);
    void compile(const Expr& expr);
    void compile(const Expression::Assign& assign);
    void compile(const Expression::Binary& binary);
    void compile(const Expression::Group& group);
    void compile(const Expression::Literal& literal);
    void compile(const Expression::Unary& unary);
    void compile(const Expression::Variable& variable);

    StringPool& pool;
    std::size_t line;
    Chunk* chunk;
    std::vector<Block> scope;
    std::vector<Variable> variables;
};

