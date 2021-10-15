#include "compiler.h"

#include <limits>
#include <shell/array.h>
#include <shell/macros.h>
#include <shell/ranges.h>

#include "dzfunction.h"
#include "dzstring.h"
#include "errors.h"

Compiler::Compiler(Interning& interning, Compiler::Type type)
    : interning(interning), type(type)
{

}

void Compiler::compile(const Tokens& tokens, Chunk& chunk)
{
    this->parser.current = tokens.begin();
    this->chunk = &chunk;
    this->scope.clear();
    this->variables.clear();

    while (!match(Token::Type::Eof))
        declaration();

    emitReturn();
}

const Compiler::Parser::Rule& Compiler::rule(Token::Type type)
{
    static constexpr auto kRulesSize = static_cast<std::size_t>(Token::Type::LastEnumValue);
    static constexpr auto kRules = shell::makeArray<Parser::Rule, kRulesSize>([](auto type) -> Parser::Rule
    {
        switch (Token::Type(type))
        {
        case Token::Type::And:          return { nullptr,             &Compiler::binary, Precedence::BitAnd     };
        case Token::Type::And2:         return { nullptr,             &Compiler::and_,   Precedence::And        };
        case Token::Type::Bang:         return { &Compiler::unary,    nullptr,           Precedence::Term       };
        case Token::Type::BangEqual:    return { nullptr,             &Compiler::binary, Precedence::Equality   };
        case Token::Type::Caret:        return { nullptr,             &Compiler::binary, Precedence::BitXor     };
        case Token::Type::Equal2:       return { nullptr,             &Compiler::binary, Precedence::Equality   };
        case Token::Type::False:        return { &Compiler::literal,  nullptr,           Precedence::None       };
        case Token::Type::Float:        return { &Compiler::constant, nullptr,           Precedence::Term       };
        case Token::Type::Greater:      return { nullptr,             &Compiler::binary, Precedence::Equality   };
        case Token::Type::Greater2:     return { nullptr,             &Compiler::binary, Precedence::BitShift   };
        case Token::Type::Greater3:     return { nullptr,             &Compiler::binary, Precedence::BitShift   };
        case Token::Type::GreaterEqual: return { nullptr,             &Compiler::binary, Precedence::Equality   };
        case Token::Type::Identifier:   return { &Compiler::variable, nullptr,           Precedence::None       };
        case Token::Type::Integer:      return { &Compiler::constant, nullptr,           Precedence::Term       };
        case Token::Type::Less:         return { nullptr,             &Compiler::binary, Precedence::Equality   };
        case Token::Type::Less2:        return { nullptr,             &Compiler::binary, Precedence::BitShift   };
        case Token::Type::LessEqual:    return { nullptr,             &Compiler::binary, Precedence::Equality   };
        case Token::Type::Minus:        return { &Compiler::unary,    &Compiler::binary, Precedence::Term       };
        case Token::Type::Null:         return { &Compiler::literal,  nullptr,           Precedence::None       };
        case Token::Type::ParenLeft:    return { &Compiler::group,    &Compiler::call,   Precedence::Assignment };
        case Token::Type::Percent:      return { nullptr,             &Compiler::binary, Precedence::Factor     };
        case Token::Type::Pipe:         return { nullptr,             &Compiler::binary, Precedence::BitOr      };
        case Token::Type::Pipe2:        return { nullptr,             &Compiler::or_,    Precedence::Or         };
        case Token::Type::Plus:         return { nullptr,             &Compiler::binary, Precedence::Term       };
        case Token::Type::Slash:        return { nullptr,             &Compiler::binary, Precedence::Factor     };
        case Token::Type::Slash2:       return { nullptr,             &Compiler::binary, Precedence::Factor     };
        case Token::Type::Star:         return { nullptr,             &Compiler::binary, Precedence::Factor     };
        case Token::Type::Star2:        return { nullptr,             &Compiler::binary, Precedence::Factor     };
        case Token::Type::String:       return { &Compiler::constant, nullptr,           Precedence::None       };
        case Token::Type::Tilde:        return { &Compiler::unary,    nullptr,           Precedence::Unary      };
        case Token::Type::True:         return { &Compiler::literal,  nullptr,           Precedence::None       };
        }
        return { nullptr, nullptr, Precedence::None };
    });
    return kRules[std::size_t(type)];
}

template<typename... Bytes>
void Compiler::emit(Bytes... bytes)
{
    (chunk->write(static_cast<u8>(bytes), parser.previous->line), ...);
}

void Compiler::emitReturn()
{
    emit(Opcode::Null, Opcode::Return);
}

void Compiler::emitConstant(DzValue value)
{
    auto index = chunk->constants.size();
    if (index <= std::numeric_limits<u8>::max())
        emit(Opcode::Constant, index);
    else if (index <= std::numeric_limits<u16>::max())
        emit(Opcode::ConstantExt, index, index >> 8);
    else
        throw CompilerError("constant limit exceeded");

    chunk->constants.push_back(value);
}

void Compiler::emitVariable(std::size_t index, Opcode opcode, Opcode opcode_ext)
{
    if (index <= std::numeric_limits<u8>::max())
        emit(opcode, index);
    else if (index <= std::numeric_limits<u16>::max())
        emit(opcode_ext, index, index >> 8);
    else
        throw CompilerError("variable limit exceeded");
}

std::size_t Compiler::emitJump(Opcode opcode, std::size_t label)
{
    auto jump = chunk->label();

    s64 offset = static_cast<s64>(label - jump) - 3; 
    if (offset < std::numeric_limits<s16>::min() || offset > -3)
        throw CompilerError("bad jump '{}'", offset);

    emit(opcode, offset, offset >> 8);

    return jump;
}

void Compiler::patchJump(std::size_t jump)
{
    s64 offset = static_cast<s64>(chunk->label() - jump) - 3;
    if (offset < 0 || offset > std::numeric_limits<s16>::max())
        throw CompilerError("bad jump '{}'", offset);

    chunk->code[jump + 1] = offset;
    chunk->code[jump + 2] = offset >> 8;
}

void Compiler::advance()
{
    parser.previous = parser.current++;
}

bool Compiler::match(Token::Type type)
{
    if (parser.current->type == type)
    {
        advance();
        return true;
    }
    return false;
}

void Compiler::parse(Precedence precedence)
{
    advance();
    auto prefix = rule(parser.previous->type).prefix;
    if (!prefix)
        throw SyntaxError(parser.previous->lexeme, "invalid syntax");

    auto can_assign = precedence <= Precedence::Assignment;
    std::invoke(prefix, this, can_assign);

    while (precedence <= rule(parser.current->type).precedence)
    {
        advance();
        auto infix = rule(parser.previous->type).infix;
        if (!infix)
            throw SyntaxError(parser.previous->lexeme, "invalid syntax");

        std::invoke(infix, this, can_assign);
    }

    if (can_assign && match(Token::Type::Equal))
        throw SyntaxError(parser.previous->lexeme, "bad assignment");
}

void Compiler::expect(Token::Type type, std::string_view error)
{
    if (!match(type))
        throw SyntaxError(parser.current->lexeme, error);
}

void Compiler::expectColon()
{
    expect(Token::Type::Colon, "expected colon");
}

void Compiler::expectDedent()
{
    expect(Token::Type::Dedent, "expected dedent");
}

void Compiler::expectIdentifier()
{
    expect(Token::Type::Identifier, "expected identifier");
}

void Compiler::expectIndent()
{
    expect(Token::Type::Indent, "expected indent");
}

void Compiler::expectNewLine()
{
    expect(Token::Type::NewLine, "expected new line");
}

void Compiler::expectParenLeft()
{
    expect(Token::Type::ParenLeft, "expected '('");
}

void Compiler::expectParenRight()
{
    expect(Token::Type::ParenRight, "expected ')'");
}

void Compiler::popLocals(std::size_t depth)
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
        throw CompilerError("too many locals to pop '{}'", count);
}

Compiler::Labels Compiler::block(const Compiler::Block& block)
{
    expectColon();
    expectNewLine();
    expectIndent();

    scope.push_back(block);

    while (parser.current->type != Token::Type::Dedent)
        declaration();

    auto breaks(std::move(scope.back().breaks));
    auto continues(std::move(scope.back().continues));

    for (const auto& jump : continues)
        patchJump(jump);

    scope.pop_back();
    popLocals(scope.size());

    expectDedent();

    return breaks;
}

void Compiler::defineVariable(std::string_view identifier)
{
    for (const auto& variable : shell::reversed(variables))
    {
        if (variable.depth != scope.size())
            break;

        if (variable.identifier == identifier)
            throw SyntaxError(identifier, "redefined '{}'", identifier);
    }
    variables.push_back({ identifier, scope.size() });
}

void Compiler::and_(bool)
{
    auto short_circuit = emitJump(Opcode::JumpFalse);
    emit(Opcode::Pop);
    parse(Precedence::And);
    patchJump(short_circuit);
}

void Compiler::binary(bool)
{
    auto token = parser.previous->type;
    parse(Precedence(int(rule(token).precedence) + 1));

    switch (token)
    {
    case Token::Type::And:          emit(Opcode::BitAnd); break;
    case Token::Type::BangEqual:    emit(Opcode::NotEqual); break;
    case Token::Type::Caret:        emit(Opcode::BitXor); break;
    case Token::Type::Equal2:       emit(Opcode::Equal); break;
    case Token::Type::Greater:      emit(Opcode::Greater); break;
    case Token::Type::Greater2:     emit(Opcode::BitAsr); break;
    case Token::Type::Greater3:     emit(Opcode::BitLsr); break;
    case Token::Type::GreaterEqual: emit(Opcode::GreaterEqual); break;
    case Token::Type::Less:         emit(Opcode::Less); break;
    case Token::Type::Less2:        emit(Opcode::BitLsl); break;
    case Token::Type::LessEqual:    emit(Opcode::LessEqual); break;
    case Token::Type::Minus:        emit(Opcode::Subtract); break;
    case Token::Type::Plus:         emit(Opcode::Add); break;
    case Token::Type::Percent:      emit(Opcode::Modulo); break;
    case Token::Type::Pipe:         emit(Opcode::BitOr); break;
    case Token::Type::Slash:        emit(Opcode::Divide); break;
    case Token::Type::Slash2:       emit(Opcode::DivideInt); break;
    case Token::Type::Star:         emit(Opcode::Multiply); break;
    case Token::Type::Star2:        emit(Opcode::Power); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Compiler::call(bool)
{
    u8 argc = 0;
    if (parser.current->type != Token::Type::ParenRight)
    {
        do
        {
            expression();
            if (argc == std::numeric_limits<u8>::max())
                throw CompilerError("Function argument limit exceeded");
            argc++;
        }
        while (match(Token::Type::Comma));
    }

    expectParenRight();
    emit(Opcode::Call, argc);
}

void Compiler::constant(bool)
{
    DzValue value;
    switch (parser.previous->value.index())
    {
    case 0: value = std::get<0>(parser.previous->value); break;
    case 1: value = std::get<1>(parser.previous->value); break;
    case 2: value = interning.make(std::get<2>(parser.previous->value)); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
    emitConstant(value);
}

void Compiler::declaration()
{
    if (match(Token::Type::Def))
        declarationDef();
    if (match(Token::Type::Var))
        declarationVar();
    else
        statement();
}

void Compiler::declarationDef()
{
    expectIdentifier();
    defineVariable(parser.previous->lexeme);

    auto function = new DzFunction();
    function->identifier = parser.previous->lexeme;

    // Prevent redefinition errors in argument list
    // Todo: check if necessary?
    scope.push_back({ Block::Type::Block });

    expectParenLeft();
    if (parser.current->type != Token::Type::ParenRight)
    {
        do
        {
            if (++function->arity > std::numeric_limits<u8>::max())
                throw CompilerError("Function argument limit exceeded");

            expectIdentifier();
            defineVariable(parser.previous->lexeme);
        }
        while (match(Token::Type::Comma));
    }
    expectParenRight();

    Compiler compiler(interning, type);
    compiler.chunk = &function->chunk;
    compiler.parser = parser;
    compiler.block({ Block::Type::Block });
    compiler.emit(Opcode::Null);
    compiler.emit(Opcode::Return);

    parser = compiler.parser;

    scope.pop_back();

    emitConstant(function);
}

void Compiler::declarationVar()
{
    expectIdentifier();
    defineVariable(parser.previous->lexeme);

    if (match(Token::Type::Equal))
        expression();
    else
        emit(Opcode::Null);

    expectNewLine();
}

void Compiler::expression()
{
    parse(Precedence::Assignment);
}

void Compiler::group(bool)
{
    expression();
    expectParenRight();
}

void Compiler::literal(bool)
{
    switch (parser.previous->type)
    {
    case Token::Type::False: emit(Opcode::False); break;
    case Token::Type::Null:  emit(Opcode::Null); break;
    case Token::Type::True:  emit(Opcode::True); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Compiler::or_(bool)
{
    auto short_circuit = emitJump(Opcode::JumpTrue);
    emit(Opcode::Pop);
    parse(Precedence::Or);
    patchJump(short_circuit);
}

void Compiler::statement()
{
    if (match(Token::Type::Assert))
        statementAssert();
    else if (match(Token::Type::Block))
        statementBlock();
    else if (match(Token::Type::Break))
        statementBreak();
    else if (match(Token::Type::Continue))
        statementContinue();
    else if (match(Token::Type::If))
        statementIf();
    else if (match(Token::Type::Noop))
        statementNoop();
    else if (match(Token::Type::Print))
        statementPrint();
    else if (match(Token::Type::Return))
        statementReturn();
    else if (match(Token::Type::While))
        statementWhile();
    else
        statementExpression();
}

void Compiler::statementAssert()
{
    expression();
    expectNewLine();
    emit(Opcode::Assert);
}

void Compiler::statementBlock()
{
    if (match(Token::Type::Identifier))
    {
        auto identifier = parser.previous->lexeme;
        for (const auto& block : scope)
        {
            if (block.identifier == identifier)
                throw SyntaxError(identifier, "redefined block '{}'", identifier);
        }

        auto breaks = block({ Block::Type::Block, identifier });
        for (const auto& jump : breaks)
            patchJump(jump);
    }
    else
    {
        block({ Block::Type::Block });
    }
}

void Compiler::statementBreak()
{
    if (match(Token::Type::Identifier))
        statementBreakBlock();
    else
        statementBreakLoop();

    expectNewLine();
}

void Compiler::statementBreakBlock()
{
    auto resolve = [this](std::string_view identifier) -> Block*
    {
        for (auto& block : shell::reversed(scope))
        {
            if (block.identifier == identifier)
                return &block;
        }
        return nullptr;
    };

    auto identifier = parser.previous->lexeme;
    auto block = resolve(identifier);
    if (!block)
        throw SyntaxError(identifier, "no matching block '{}'", identifier);
    
    popLocals(std::distance(scope.data(), block));
    block->breaks.push_back(emitJump(Opcode::Jump));
}

void Compiler::statementBreakLoop()
{
    auto resolve = [this]() -> Block*
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
        throw SyntaxError(parser.previous->lexeme, "'break' outside loop");

    popLocals(std::distance(scope.data(), block));
    block->breaks.push_back(emitJump(Opcode::Jump));
}

void Compiler::statementContinue()
{
    auto resolve = [this]() -> Block*
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
        throw SyntaxError(parser.previous->lexeme, "'continue' outside loop");

    block->continues.push_back(emitJump(Opcode::Jump));

    expectNewLine();
}

void Compiler::statementExpression()
{
    expression();
    expectNewLine();
    emit(Opcode::Pop);
}

void Compiler::statementIf()
{
    Labels exits;
    do
    {
        expression();
        auto elif = emitJump(Opcode::JumpFalsePop);
        block({ Block::Type::Conditional });
        exits.push_back(emitJump(Opcode::Jump));
        patchJump(elif);
    }
    while (match(Token::Type::Elif));

    if (match(Token::Type::Else))
        block({ Block::Type::Conditional });

    for (const auto& jump : exits)
        patchJump(jump);
}

void Compiler::statementNoop()
{
    expectNewLine();
}

void Compiler::statementPrint()
{
    expression();
    expectNewLine();
    emit(Opcode::Print);
}

void Compiler::statementReturn()
{
    if (type == Type::Main)
        throw SyntaxError(parser.previous->lexeme, "return outside function");

    if (match(Token::Type::NewLine))
    {
        emitReturn();
    }
    else
    {
        expression();
        expectNewLine();
        emit(Opcode::Return);
    }
}

void Compiler::statementWhile()
{
    auto condition = chunk->label();
    expression();
    auto exit = emitJump(Opcode::JumpFalsePop);
    auto breaks = block({ Block::Type::Loop });
    emitJump(Opcode::Jump, condition);

    patchJump(exit);
    for (const auto& jump : breaks)
        patchJump(jump);
}

void Compiler::unary(bool)
{
    auto token = parser.previous->type;
    parse(Precedence::Unary);

    switch (token)
    {
    case Token::Type::Bang:  emit(Opcode::Not); break;
    case Token::Type::Minus: emit(Opcode::Negate); break;
    case Token::Type::Tilde: emit(Opcode::BitComplement); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Compiler::variable(bool can_assign)
{
    auto resolve = [this](std::string_view identifier) -> Variable*
    {
        for (auto& variable : shell::reversed(variables))
        {
            if (variable.identifier == identifier)
                return &variable;
        }
        return nullptr;
    };

    auto identifier = parser.previous->lexeme;
    auto variable = resolve(identifier);
    if (!variable)
        throw SyntaxError(identifier, "undefined variable '{}'", identifier);

    auto index = std::distance(variables.data(), variable);
    if (can_assign && match(Token::Type::Equal))
    {
        expression();
        emitVariable(index, Opcode::StoreVariable, Opcode::StoreVariableExt);
    }
    else
    {
        emitVariable(index, Opcode::LoadVariable, Opcode::LoadVariableExt);
    }
}
