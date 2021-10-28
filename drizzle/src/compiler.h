#pragma once

#include "chunk.h"
#include "statement.h"
#include "stringpool.h"

class Compiler
{
public:
    Compiler(StringPool& pool);

    void compile(const Stmt& ast, Chunk& chunk);

private:
    template<typename... Bytes>
    void emit(Bytes... bytes);
    void emitConstant(DzValue value);

    void compile(const Stmt& stmt);
    void compile(const Statement::Block& block);
    void compile(const Statement::ExpressionStatement& expression_statement);
    void compile(const Statement::Print& print);
    void compile(const Statement::Program& program);
    void compile(const Statement::Var& variable_definition);
    void compile(const Expr& expr);
    void compile(const Expression::Binary& binary);
    void compile(const Expression::Group& group);
    void compile(const Expression::Literal& literal);
    void compile(const Expression::Unary& unary);

    StringPool& pool;
    std::size_t line;
    Chunk* chunk;
};

