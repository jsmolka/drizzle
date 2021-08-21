#include "scanner.h"

#include <shell/format.h>

#include "error.h"

std::vector<Token> Scanner::scan(const std::string& source)
{
    cursor = source.data();
    lexeme = source.data();
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
                throw SyntaxError("tab in indent", cursor);

            default:
                return spaces;
            }
        }
    };

    int spaces = count_spaces();
    if (spaces % kSpacesPerIndentation)
        throw SyntaxError("invalid indent", cursor);

    int indent = spaces / kSpacesPerIndentation;
    if (indent > indentation)
    {
        if ((indent - indentation) > 1)
            throw SyntaxError("unexpected indent", cursor);

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
    const char* begin = cursor;

    if (next(R"("")"))
    {
        while (*cursor)
        {
            if (next(R"(""")"))
            {
                begin = nullptr;
                break;
            }
            next();
        }
    }
    else
    {
        while (*cursor)
        {
            if (next("\""))
            {
                begin = nullptr;
                break;
            }

            switch (*cursor)
            {
            case '\n':
                throw SyntaxError("line break in string", cursor);

            case '\\':
                next();
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
                    throw SyntaxError("invalid escape sequence", cursor);
                }
                break;

            default:
                next();
                break;
            }
        }
    }

    if (begin)
        throw SyntaxError("unterminated string", begin);

    emit(Token::Type::String);
}

void Scanner::scanNumber()
{
    const auto is_bin = isDigit<2>;
    const auto is_hex = isDigit<16>;

    if (cursor[-1] == '0' && next("b"))
    {
        if (!is_bin(next()))
            throw SyntaxError("invalid bin literal", cursor);

        while (is_bin(*cursor))
            next();

        emit(Token::Type::Integer);
        return;
    }
    
    if (cursor[-1] == '0' && next("x"))
    {
        if (!is_hex(next()))
            throw SyntaxError("invalid hex literal", cursor);

        while (is_hex(*cursor))
            next();

        emit(Token::Type::Integer);
        return;
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