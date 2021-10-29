#include "compiler.h"

#include <shell/macros.h>
#include <shell/ranges.h>

#include "errors.h"
#include "opcode.h"

Compiler::Compiler(StringPool& pool)
    : pool(pool)
{
}

void Compiler::compile(const Stmt& ast, Chunk& chunk)
{
    this->chunk = &chunk;

    scope.push_back({ Block::Type::Block });
    compile(ast);
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

void Compiler::compile(const Stmt& stmt)
{
    static_assert(int(Statement::Type::LastEnumValue) == 6);

    line = stmt->location.line;

    switch (stmt->type)
    {
    case Statement::Type::Noop: break; 
    case Statement::Type::Block:               compile(stmt->block); break;
    case Statement::Type::ExpressionStatement: compile(stmt->expression_statement); break;
    case Statement::Type::Print:               compile(stmt->print); break;
    case Statement::Type::Program:             compile(stmt->program); break;
    case Statement::Type::Var:                 compile(stmt->var); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Compiler::compile(const Statement::Block& block)
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

    for (const auto& stmt : block.statements)
        compile(stmt);

    scope.pop_back();

    popVariables(scope.size());
}

void Compiler::compile(const Statement::ExpressionStatement& expression_statement)
{
    compile(expression_statement.expression);
    emit(Opcode::Pop);
}

void Compiler::compile(const Statement::Print& print)
{
    compile(print.expression);
    emit(Opcode::Print);
}

void Compiler::compile(const Statement::Program& program)
{
    for (const auto& stmt : program.statements)
        compile(stmt);
}

void Compiler::compile(const Statement::Var& var)
{
    compile(var.initializer);
    defineVariable(var.identifier);
}

void Compiler::compile(const Expr& expr)
{
    static_assert(int(Expression::Type::LastEnumValue) == 6);

    line = expr->location.line;

    switch (expr->type)
    {
    case Expression::Type::Assign:   compile(expr->assign); break;
    case Expression::Type::Binary:   compile(expr->binary); break;
    case Expression::Type::Group:    compile(expr->group); break;
    case Expression::Type::Literal:  compile(expr->literal); break;
    case Expression::Type::Unary:    compile(expr->unary); break;
    case Expression::Type::Variable: compile(expr->variable); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Compiler::compile(const Expression::Assign& assign)
{
    compile(assign.value);

    const auto& var = resolveVariable(assign.identifier);
    const auto index = std::distance<const Variable*>(variables.data(), &var);
    emitVariable(index, Opcode::StoreVariable);
}

void Compiler::compile(const Expression::Binary& binary)
{
    static_assert(int(Expression::Binary::Type::LastEnumValue) == 21);

    if (binary.type == Expression::Binary::Type::And)
    {
        compile(binary.left);
        const auto short_circuit = emitJump(Opcode::JumpFalse);
        emit(Opcode::Pop);
        compile(binary.right);
        patchJump(short_circuit);
        return;
    }
    
    if (binary.type == Expression::Binary::Type::Or)
    {
        compile(binary.left);
        const auto short_circuit = emitJump(Opcode::JumpTrue);
        emit(Opcode::Pop);
        compile(binary.right);
        patchJump(short_circuit);
        return;
    }

    compile(binary.left);
    compile(binary.right);

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

void Compiler::compile(const Expression::Group& group)
{
    compile(group.expression);
}

void Compiler::compile(const Expression::Literal& literal)
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

void Compiler::compile(const Expression::Unary& unary)
{
    static_assert(int(Expression::Unary::Type::LastEnumValue) == 3);

    compile(unary.expression);

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

void Compiler::compile(const Expression::Variable& variable)
{
    const auto& var = resolveVariable(variable.identifier);
    const auto index = std::distance<const Variable*>(variables.data(), &var);
    emitVariable(index, Opcode::LoadVariable);
}
