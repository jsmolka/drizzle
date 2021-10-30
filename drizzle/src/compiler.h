#pragma once

#include "astwalker.h"
#include "chunk.h"
#include "opcode.h"
#include "statement.h"
#include "stringpool.h"

class Compiler final : public AstWalker
{
public:
    Compiler(StringPool& pool);

    void compile(Stmt& ast, Chunk& chunk);

private:
    struct Block
    {
        enum class Type { Block, Branch, Loop, Function };

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
    std::size_t emitJump(Opcode opcode, std::size_t label = 0);
    void patchJump(std::size_t jump);

    void defineVariable(std::string_view identifier);
    Variable& resolveVariable(std::string_view identifier);
    void popVariables(std::size_t depth);

    void walk(Stmt& stmt) final;
    void walk(Statement::Block& block) final;
    void walk(Statement::ExpressionStatement& expression_statement) final;
    void walk(Statement::If& if_) final;
    void walk(Statement::Print& print) final;
    void walk(Statement::Var& var) final;
    void walk(Statement::While& while_) final;

    void walk(Expr& expr) final;
    void walk(Expression::Assign& assign) final;
    void walk(Expression::Binary& binary) final;
    void walk(Expression::Literal& literal) final;
    void walk(Expression::Unary& unary) final;
    void walk(Expression::Variable& variable) final;

    StringPool& pool;
    std::size_t line;
    Chunk* chunk;
    std::vector<Block> scope;
    std::vector<Variable> variables;
};

