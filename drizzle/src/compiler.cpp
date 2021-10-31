#include "compiler.h"

#include <shell/macros.h>
#include <shell/ranges.h>

#include "errors.h"
#include "opcode.h"

Compiler::Compiler(StringPool& pool)
    : pool(pool)
{
}

void Compiler::compile(Stmt& ast, Chunk& chunk)
{
    this->chunk = &chunk;

    scope.push_back({ Block::Type::Block });
    walk(ast);
    scope.pop_back();
    popVariables(0);

    emit(Opcode::Return);
}

template<typename ...Bytes>
void Compiler::emit(Bytes ...bytes)
{
    (chunk->write(static_cast<u8>(bytes), line), ...);
}

void Compiler::emitConstant(DzValue value)
{
    const auto index = chunk->constants.size();
    if (index <= std::numeric_limits<u8>::max())
        emit(Opcode::Constant, index);
    else if (index <= std::numeric_limits<u16>::max())
        emit(Opcode::ConstantExt, index, index >> 8);
    else
        throw CompilerError(line, "constant limit exceeded");

    chunk->constants.push_back(value);
}

void Compiler::emitVariable(std::size_t index, Opcode opcode)
{
    if (index <= std::numeric_limits<u8>::max())
        emit(opcode, index);
    else if (index <= std::numeric_limits<u16>::max())
        emit(Opcode(int(opcode) + 1), index, index >> 8);
    else
        throw CompilerError(line, "variable limit exceeded");
}

std::size_t Compiler::emitJump(Opcode opcode, std::size_t label)
{
    const auto jump = chunk->label();

    s64 offset = static_cast<s64>(label - jump) - 3; 
    if (offset < std::numeric_limits<s16>::min() || offset > -3)
        throw CompilerError(line, "bad jump '{}'", offset);

    emit(opcode, offset, offset >> 8);

    return jump;
}

void Compiler::patchJump(std::size_t jump)
{
    s64 offset = static_cast<s64>(chunk->label() - jump) - 3;
    if (offset < 0 || offset > std::numeric_limits<s16>::max())
        throw CompilerError(line, "bad jump '{}'", offset);

    chunk->code[jump + 1] = offset;
    chunk->code[jump + 2] = offset >> 8;
}

void Compiler::defineVariable(std::string_view identifier)
{
    for (const auto& variable : shell::reversed(variables))
    {
        if (variable.depth != scope.size())
            break;

        if (variable.identifier == identifier)
            throw SyntaxError(identifier.data(), "redefined '{}'", identifier);
    }
    variables.push_back({ identifier, scope.size() });
}

Compiler::Variable& Compiler::resolveVariable(std::string_view identifier)
{
    for (auto& variable : shell::reversed(variables))
    {
        if (variable.identifier == identifier)
            return variable;
    }

    throw SyntaxError(identifier.data(), "undefined variable '{}'", identifier);
}

void Compiler::popVariables(std::size_t depth)
{
    auto size = variables.size();
    while (variables.size() && variables.back().depth > depth)
        variables.pop_back();

    auto count = size - variables.size();
    if (count == 0)
        return;

    if (count == 1)
        emit(Opcode::Pop);
    else if (count <= std::numeric_limits<u8>::max())
        emit(Opcode::PopMultiple, count);
    else if (count <= std::numeric_limits<u16>::max())
        emit(Opcode::PopMultipleExt, count, count >> 8);
    else
        throw CompilerError(line, "too many variables to pop '{}'", count);
}

void Compiler::walk(Stmt& stmt)
{
    line = stmt->location.line;
    AstWalker::walk(stmt);
}

void Compiler::walk(Statement::Block& block)
{
    if (block.identifier.size())
    {
        for (const auto& scope_block : scope)
        {
            if (block.identifier == scope_block.identifier)
                throw SyntaxError(block.identifier.data(), "redefined block '{}'", block.identifier);
        }
    }

    scope.push_back({ Block::Type::Block, block.identifier });
    AstWalker::walk(block);
    scope.pop_back();

    popVariables(scope.size());
}

void Compiler::walk(Statement::Break& break_)
{
}

void Compiler::walk(Statement::Continue& continue_)
{
    const auto resolve = [this]() -> Block*
    {
        for (auto& block : shell::reversed(scope))
        {
            if (block.type == Block::Type::Loop)
                return &block;
        }
        return nullptr;
    };

    auto block = resolve();
    if (!block)
        throw SyntaxError(line, "'continue' outside loop");

    block->continues.push_back(emitJump(Opcode::Jump));
}

void Compiler::walk(Statement::ExpressionStatement& expression_statement)
{
    AstWalker::walk(expression_statement);
    emit(Opcode::Pop);
}

void Compiler::walk(Statement::If& if_)
{
    std::vector<std::size_t> exits;

    const auto branch = [this, &exits](Statement::If::Branch& branch)
    {
        walk(branch.condition);
        const auto next = emitJump(Opcode::JumpFalsePop);
        scope.push_back({ Block::Type::Branch });
        AstWalker::walk(branch.statements);
        scope.pop_back();
        exits.push_back(emitJump(Opcode::Jump));
        patchJump(next);
    };

    branch(if_.if_);
    for (auto& elif : if_.elifs)
        branch(elif);

    AstWalker::walk(if_.else_);

    for (const auto& exit : exits)
        patchJump(exit);
}

void Compiler::walk(Statement::Print& print)
{
    AstWalker::walk(print);
    emit(Opcode::Print);
}

void Compiler::walk(Statement::Var& var)
{
    AstWalker::walk(var);
    defineVariable(var.identifier);
}

void Compiler::walk(Statement::While& while_)
{
    const auto condition = chunk->label();
    walk(while_.condition);
    const auto exit = emitJump(Opcode::JumpFalsePop);
    scope.push_back({ Block::Type::Loop });
    AstWalker::walk(while_.statements);
    for (const auto& continue_ : scope.back().continues)
        patchJump(continue_);
    scope.pop_back();
    emitJump(Opcode::Jump, condition);
    popVariables(scope.size());
    patchJump(exit);
}

void Compiler::walk(Expr& expr)
{
    line = expr->location.line;
    AstWalker::walk(expr);
}

void Compiler::walk(Expression::Assign& assign)
{
    AstWalker::walk(assign);
    const auto& var = resolveVariable(assign.identifier);
    const auto index = std::distance<const Variable*>(variables.data(), &var);
    emitVariable(index, Opcode::StoreVariable);
}

void Compiler::walk(Expression::Binary& binary)
{
    static_assert(int(Expression::Binary::Type::LastEnumValue) == 21);

    if (binary.type == Expression::Binary::Type::And)
    {
        walk(binary.left);
        const auto short_circuit = emitJump(Opcode::JumpFalse);
        emit(Opcode::Pop);
        walk(binary.right);
        patchJump(short_circuit);
        return;
    }
    
    if (binary.type == Expression::Binary::Type::Or)
    {
        walk(binary.left);
        const auto short_circuit = emitJump(Opcode::JumpTrue);
        emit(Opcode::Pop);
        walk(binary.right);
        patchJump(short_circuit);
        return;
    }

    AstWalker::walk(binary);

    switch (binary.type)
    {
    case Expression::Binary::Type::Addition:        emit(Opcode::Add); break;
    case Expression::Binary::Type::BitwiseAnd:      emit(Opcode::BitwiseAnd); break;
    case Expression::Binary::Type::BitwiseAsr:      emit(Opcode::BitwiseAsr); break;
    case Expression::Binary::Type::BitwiseLsl:      emit(Opcode::BitwiseLsl); break;
    case Expression::Binary::Type::BitwiseLsr:      emit(Opcode::BitwiseLsr); break;
    case Expression::Binary::Type::BitwiseOr:       emit(Opcode::BitwiseOr); break;
    case Expression::Binary::Type::BitwiseXor:      emit(Opcode::BitwiseXor); break;
    case Expression::Binary::Type::Division:        emit(Opcode::Divide); break;
    case Expression::Binary::Type::Equal:           emit(Opcode::Equal); break;
    case Expression::Binary::Type::Greater:         emit(Opcode::Greater); break;
    case Expression::Binary::Type::GreaterEqual:    emit(Opcode::GreaterEqual); break;
    case Expression::Binary::Type::IntegerDivision: emit(Opcode::DivideInt); break;
    case Expression::Binary::Type::Less:            emit(Opcode::Less); break;
    case Expression::Binary::Type::LessEqual:       emit(Opcode::LessEqual); break;
    case Expression::Binary::Type::Modulo:          emit(Opcode::Modulo); break;
    case Expression::Binary::Type::Multiplication:  emit(Opcode::Multiply); break;
    case Expression::Binary::Type::NotEqual:        emit(Opcode::NotEqual); break;
    case Expression::Binary::Type::Power:           emit(Opcode::Power); break;
    case Expression::Binary::Type::Subtraction:     emit(Opcode::Subtract); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Compiler::walk(Expression::Literal& literal)
{
    static_assert(int(Expression::Literal::Type::LastEnumValue) == 5);

    switch (literal.type)
    {
    case Expression::Literal::Type::Null:
        emit(Opcode::Null);
        break;

    case Expression::Literal::Type::Boolean:
        emit(std::get<dzbool>(literal.value) ? Opcode::True : Opcode::False);
        break;

    case Expression::Literal::Type::Float:
        emitConstant(std::get<dzfloat>(literal.value));
        break;
        
    case Expression::Literal::Type::Integer:
        emitConstant(std::get<dzint>(literal.value));
        break;

    case Expression::Literal::Type::String:
        emitConstant(pool.make(std::string(std::get<std::string>(literal.value))));
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Compiler::walk(Expression::Unary& unary)
{
    static_assert(int(Expression::Unary::Type::LastEnumValue) == 3);

    AstWalker::walk(unary);

    switch (unary.type)
    {
    case Expression::Unary::Type::BitwiseComplement: emit(Opcode::BitwiseComplement); break;
    case Expression::Unary::Type::Minus:             emit(Opcode::Negate); break;
    case Expression::Unary::Type::Not:               emit(Opcode::Not); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Compiler::walk(Expression::Variable& variable)
{
    const auto& var = resolveVariable(variable.identifier);
    const auto index = std::distance<const Variable*>(variables.data(), &var);
    emitVariable(index, Opcode::LoadVariable);
}
