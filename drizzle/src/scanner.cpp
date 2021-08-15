#include "scanner.h"

#include "error.h"

std::vector<Token> Scanner::scan(const std::string& source)
{
    cursor = source.data();
    lexeme = source.data();
    line = 1;
    indentation = 0;

    tokens.clear();

    blankLines();
    doIndentation();

    while (!isFileEnd())
    {
        token();
        whitespace();
    }

    while (indentation > 0)
    {
        emit(Token::Type::Dedent);
        indentation--;
    }

    emit(Token::Type::Eof);

    return tokens;
}

char Scanner::next()
{
    cursor++;
    return cursor[-1];
}

char Scanner::peek() const
{
    return *cursor;
}

char Scanner::peekNext() const
{
    return isFileEnd() ? '\0' : cursor[1];
}

bool Scanner::match(char expected)
{
    if (isFileEnd() || *cursor != expected)
        return false;

    cursor++;
    return true;
}

void Scanner::emit(Token::Type type)
{
    Token token;
    token.type = type;
    token.lexeme = std::string_view(lexeme, cursor - lexeme);
    token.line = line;
    tokens.push_back(token);

    lexeme = cursor;
}

void Scanner::doIndentation()
{
    constexpr auto kSpacesPerIndentation = 2;

    switch (peek())
    {
    case ' ':
    case '\r':
    case '\t':
        break;

    default:
        while (indentation > 0)
        {
            emit(Token::Type::Dedent);
            indentation--;
        }
        return;
    }

    auto count_spaces = [this]() -> int
    {
        int spaces = 0;
        while (true)
        {
            switch (peek())
            {
            case ' ':
                spaces++;
                next();
                break;

            case '\r':
                next();
                break;

            case '\t':
                throw SyntaxError("cannot use tabs for indentation");

            default:
                return spaces;
            }
        }
    };

    int spaces = count_spaces();
    if (spaces % kSpacesPerIndentation)
        throw SyntaxError("indentation spaces must be a multiple of {}", kSpacesPerIndentation);

    int indent = spaces / kSpacesPerIndentation;
    if (indent > indentation)
    {
        if ((indent - indentation) > 1)
            throw SyntaxError("cannot indent {} levels at once", indent - indentation);

        emit(Token::Type::Indent);
        indentation++;
    }
    else
    {
        while (indent < indentation)
        {
            emit(Token::Type::Dedent);
            indentation--;
        }
    }
}

void Scanner::blankLines()
{
    auto skip_line = [this]() -> bool
    {
        const char* previous = cursor;
        while (!isFileEnd())
        {
            switch (peek())
            {
            case ' ':
            case '\r':
            case '\t':
                next();
                break;

            case '#':
                comment();
                break;

            case '\n':
                next();
                line++;
                lexeme = cursor;
                return true;

            default:
                cursor = previous;
                return false;
            }
        }
        return false;
    };

    while (skip_line());
}

void Scanner::whitespace()
{
    while (true)
    {
        switch (peek())
        {
        case ' ':
        case '\r':
        case '\t':
            next();
            break;

        case '#':
            comment();
            break;

        case '\n':
            next();
            emit(Token::Type::NewLine);
            line++;
            blankLines();
            doIndentation();
            return;

        default:
            return;
        }
    }
}

void Scanner::comment()
{
    while (!isFileEnd() && peek() != '\n')
        next();
}

bool Scanner::isFileEnd() const
{
    return *cursor == '\0';
}

void Scanner::token()
{
        //// Literals
        //Float, Identifier, Integer, String,

        //// Keywords
        //Break, Class, Continue, Elif, Else, Extends, False, For,
        //Function, If, In, Iterator, Noop, Null, Return, Super,
        //This, True, Var, While, Yield,

        //// Special
        //Eof, Indent,

        //// Todo: remove when functions are implemented
        //Print

    switch (next())
    {
    // Single
    case '{': return emit(Token::Type::BraceLeft);
    case '}': return emit(Token::Type::BraceRight);
    case '[': return emit(Token::Type::BracketLeft);
    case ']': return emit(Token::Type::BracketRight);
    case '^': return emit(Token::Type::Caret);
    case ':': return emit(Token::Type::Colon);
    case ',': return emit(Token::Type::Comma);
    case '.': return emit(Token::Type::Dot);
    case '-': return emit(Token::Type::Minus);
    case '(': return emit(Token::Type::ParenLeft);
    case ')': return emit(Token::Type::ParenRight);
    case '%': return emit(Token::Type::Percent);
    case '+': return emit(Token::Type::Plus);
    case '/': return emit(Token::Type::Slash);
    case '*': return emit(Token::Type::Star);

    // Single or double
    case '&': return emit(match('&') ? Token::Type::AndAnd : Token::Type::And);
    case '!': return emit(match('=') ? Token::Type::BangEqual : Token::Type::Bang);
    case '=': return emit(match('=') ? Token::Type::EqualEqual : Token::Type::Equal);
    case '>': return emit(match('=') ? Token::Type::GreaterEqual : Token::Type::Greater);
    case '<': return emit(match('=') ? Token::Type::LessEqual : Token::Type::Less);
    case '|': return emit(match('|') ? Token::Type::PipePipe : Token::Type::Pipe);
    }
}
