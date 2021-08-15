#include "scanner.h"

#include "error.h"

std::vector<Token> Scanner::scan(const std::string& source)
{
    cursor = source.data();
    lexeme = source.data();
    line = 1;
    indentation = 0;
    tokens.clear();

    scanBlankLines();
    scanIndentation();

    while (!isEof())
    {
        scanToken();
        scanWhitespace();
    }

    if (indentation > 0)
    {
        if (tokens.back().type != Token::Type::NewLine)
            emit(Token::Type::NewLine);

        while (indentation--)
            emit(Token::Type::Dedent);
    }

    emit(Token::Type::Eof);

    return tokens;
}

bool Scanner::isEof() const
{
    return *cursor == '\0';
}

char Scanner::next()
{
    cursor++;
    return cursor[-1];
}

bool Scanner::next(char expect)
{
    if (isEof() || *cursor != expect)
        return false;

    cursor++;
    return true;
}

char Scanner::peek() const
{
    return *cursor;
}

char Scanner::peekNext() const
{
    return isEof() ? '\0' : cursor[1];
}

void Scanner::emit(Token::Type type)
{
    Token token;
    token.type = type;
    token.line = line;
    token.lexeme = std::string_view(lexeme, cursor - lexeme);

    lexeme = cursor;
    tokens.push_back(token);
}

void Scanner::scanIndentation()
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

void Scanner::scanBlankLines()
{
    auto skip_line = [this]() -> bool
    {
        const char* previous = cursor;
        while (!isEof())
        {
            switch (peek())
            {
            case ' ':
            case '\r':
            case '\t':
                next();
                break;

            case '#':
                scanComment();
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

void Scanner::scanWhitespace()
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
            scanComment();
            break;

        case '\n':
            next();
            line++;
            emit(Token::Type::NewLine);
            scanBlankLines();
            scanIndentation();
            return;

        default:
            return;
        }
    }
}

void Scanner::scanComment()
{
    while (!isEof() && peek() != '\n')
        next();
}

void Scanner::scanToken()
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
    case '&': return emit(next('&') ? Token::Type::AndAnd : Token::Type::And);
    case '!': return emit(next('=') ? Token::Type::BangEqual : Token::Type::Bang);
    case '=': return emit(next('=') ? Token::Type::EqualEqual : Token::Type::Equal);
    case '>': return emit(next('=') ? Token::Type::GreaterEqual : Token::Type::Greater);
    case '<': return emit(next('=') ? Token::Type::LessEqual : Token::Type::Less);
    case '|': return emit(next('|') ? Token::Type::PipePipe : Token::Type::Pipe);
    }
}
