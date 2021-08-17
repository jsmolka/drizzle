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
    lexeme = cursor;

    while (*cursor)
    {
        scanToken();
        scanWhitespace();
        lexeme = cursor;
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

template<unsigned Base>
bool Scanner::isDigit(char c)
{
    static_assert(Base <= 36);

    if constexpr (Base <= 10)
    {
        return (c >= '0') && (c < ('0' + Base));
    }
    if constexpr (Base <= 36)
    {
        return (c >= '0' && c <= '9')
            || (c >= 'a' && c < ('a' + Base - 10))
            || (c >= 'A' && c < ('A' + Base - 10));
    }
    return false;
}

bool Scanner::isAlpha(char c)
{
    return (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z')
        || (c == '_');
}

char Scanner::next()
{
    cursor++;
    return cursor[-1];
}

bool Scanner::next(std::string_view match)
{
    const char* previous = cursor;
    for (char c : match)
    {
        if (!(*cursor && *cursor == c))
        {
            cursor = previous;
            return false;
        }
        cursor++;
    }
    return true;
}

char Scanner::peek() const
{
    return *cursor ? cursor[1] : *cursor;
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

    switch (*cursor)
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
            switch (*cursor)
            {
            case ' ':
                spaces++;
                next();
                break;

            case '\r':
                next();
                break;

            case '\t':
                throw SyntaxError("tab in indent");

            default:
                return spaces;
            }
        }
    };

    int spaces = count_spaces();
    if (spaces % kSpacesPerIndentation)
        throw SyntaxError("invalid indent");

    int indent = spaces / kSpacesPerIndentation;
    if (indent > indentation)
    {
        if ((indent - indentation) > 1)
            throw SyntaxError("unpected indent");

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
        while (*cursor)
        {
            switch (*cursor)
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
        switch (*cursor)
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
    while (*cursor && *cursor != '\n')
        next();
}

void Scanner::scanString()
{
    bool terminated = false;

    if (next(R"("")"))
    {
        while (*cursor)
        {
            terminated = next(R"(""")");
            if (terminated)
                break;

            if (*cursor == '\n')
                line++;

            next();
        }
    }
    else
    {
        while (*cursor)
        {
            terminated = next("\"");
            if (terminated)
                break;

            switch (next())
            {
            case '\n':
                throw SyntaxError("invalid string");

            case '\\':
                switch (*cursor)
                {
                case '\\':
                case '\"':
                case 'a':
                case 'b':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                case 'v':
                    next();
                    break;

                default:
                    throw SyntaxError("invalid escape sequence");
                }
                break;
            }
        }
    }

    if (!terminated)
        throw SyntaxError("unterminated string");

    emit(Token::Type::String);
}

void Scanner::scanNumber()
{
    if (cursor[-1] == '0' && next("b"))
    {
        if (!isDigit<2>(next()))
            throw SyntaxError("invalid binary integer literal");

        while (isDigit<2>(*cursor))
            next();

        return emit(Token::Type::Integer);
    }
    
    if (cursor[-1] == '0' && next("x"))
    {
        if (!isDigit<16>(*cursor))
            throw SyntaxError("invalid hexadecimal integer literal");

        while (isDigit<16>(*cursor))
            next();

        return emit(Token::Type::Integer);
    }
}

void Scanner::scanToken()
{
    //// Literals
    //Float, Identifier, Integer, String,

    //// Keywords
    //Break, Class, Continue, Elif, Else, Extends, False, For,
    //Function, If, In, Iterator, Noop, Null, Return, Super,
    //This, True, Var, While, Yield,

    //// Todo: remove when functions are implemented
    //Print

    char c = next();

    if (isDigit(c))
    {
        scanNumber();
        return;
    }

    switch (c)
    {
        // Single
    case '{': emit(Token::Type::BraceLeft); break;
    case '}': emit(Token::Type::BraceRight); break;
    case '[': emit(Token::Type::BracketLeft); break;
    case ']': emit(Token::Type::BracketRight); break;
    case '^': emit(Token::Type::Caret); break;
    case ':': emit(Token::Type::Colon); break;
    case ',': emit(Token::Type::Comma); break;
    case '.': emit(Token::Type::Dot); break;
    case '-': emit(Token::Type::Minus); break;
    case '(': emit(Token::Type::ParenLeft); break;
    case ')': emit(Token::Type::ParenRight); break;
    case '%': emit(Token::Type::Percent); break;
    case '+': emit(Token::Type::Plus); break;
    case '/': emit(Token::Type::Slash); break;
    case '*': emit(Token::Type::Star); break;

        // Single or double
    case '&': emit(next("&") ? Token::Type::AndAnd : Token::Type::And); break;
    case '!': emit(next("=") ? Token::Type::BangEqual : Token::Type::Bang); break;
    case '=': emit(next("=") ? Token::Type::EqualEqual : Token::Type::Equal); break;
    case '>': emit(next("=") ? Token::Type::GreaterEqual : Token::Type::Greater); break;
    case '<': emit(next("=") ? Token::Type::LessEqual : Token::Type::Less); break;
    case '|': emit(next("|") ? Token::Type::PipePipe : Token::Type::Pipe); break;

        // Special
    case '"': scanString(); break;
    }
}