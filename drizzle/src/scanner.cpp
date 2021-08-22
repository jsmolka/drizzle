#include "scanner.h"

#include <tuple>

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

    emit(Token::Type::Eof);

    return tokens;
}

template<char Base>
bool Scanner::isDigit(char c)
{
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

bool Scanner::next(char match)
{
    if (*cursor && *cursor == match)
    {
        cursor++;
        return true;
    }
    return false;
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

    const char* begin = cursor;

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

    auto count_spaces = [this, begin]() -> int
    {
        int spaces = 0;
        while (true)
        {
            switch (*cursor)
            {
            case ' ':
                next();
                spaces++;
                break;

            case '\r':
                next();
                break;

            case '\t':
                throw SyntaxError("tab indent", begin);

            default:
                return spaces;
            }
        }
    };

    int spaces = count_spaces();
    if (spaces % kSpacesPerIndentation)
        throw SyntaxError("uneven indent", begin);

    int indent = spaces / kSpacesPerIndentation;
    if (indent > indentation)
    {
        if ((indent - indentation) > 1)
            throw SyntaxError("unexpected indent", begin);

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

    if (next(R"(""")"))
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
        next();

        while (*cursor)
        {
            if (next('"'))
            {
                begin = nullptr;
                break;
            }

            switch (*cursor)
            {
            case '\n':
                throw SyntaxError("unexpected line break", cursor);

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
                    throw SyntaxError("unknown escape sequence", cursor);
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
    const auto is_dec = isDigit<10>;
    const auto is_hex = isDigit<16>;

    if (next("0b"))
    {
        if (!is_bin(*cursor))
            throw SyntaxError("expected bin digit", cursor);

        while (is_bin(*cursor))
            next();

        emit(Token::Type::Integer);
        return;
    }
    
    if (next("0x"))
    {
        if (!is_hex(*cursor))
            throw SyntaxError("expected hex digit", cursor);

        while (is_hex(*cursor))
            next();

        emit(Token::Type::Integer);
        return;
    }

    if (*cursor == '0' && is_dec(peek()))
        throw SyntaxError("unexpected zero", cursor);

    while (is_dec(*cursor))
        next();

    if (next('.'))
    {
        if (!is_dec(*cursor))
            throw SyntaxError("expected digit", cursor);

        while (is_dec(*cursor))
            next();

        emit(Token::Type::Float);
    }
    else
    {
        emit(Token::Type::Integer);
    }
}

void Scanner::scanIdentifier()
{
    static constexpr std::tuple<std::string_view, Token::Type> kKeywords[] =
    {
        { "assert",   Token::Type::Assert   },
        { "break",    Token::Type::Break    },
        { "class",    Token::Type::Class    },
        { "continue", Token::Type::Continue },
        { "elif",     Token::Type::Elif     },
        { "else",     Token::Type::Else     },
        { "extends",  Token::Type::Extends  },
        { "false",    Token::Type::False    },
        { "for",      Token::Type::For      },
        { "function", Token::Type::Function },
        { "if",       Token::Type::If       },
        { "in",       Token::Type::In       },
        { "iterator", Token::Type::Iterator },
        { "noop",     Token::Type::Noop     },
        { "null",     Token::Type::Null     },
        { "print",    Token::Type::Print    },
        { "return",   Token::Type::Return   },
        { "super",    Token::Type::Super    },
        { "this",     Token::Type::This     },
        { "true",     Token::Type::True     },
        { "var",      Token::Type::Var      },
        { "while",    Token::Type::While    },
        { "yield",    Token::Type::Yield    }
    };

    while (isAlpha(*cursor) || isDigit(*cursor))
        next();

    std::string_view identifier(lexeme, cursor - lexeme);

    for (const auto& [keyword, type] : kKeywords)
    {
        if (identifier == keyword)
        {
            emit(type);
            return;
        }
    }
    
    emit(Token::Type::Identifier);
}

void Scanner::scanToken()
{
    if (isAlpha(*cursor))
    {
        scanIdentifier();
        return;
    }

    if (isDigit(*cursor))
    {
        scanNumber();
        return;
    }

    if (*cursor == '"')
    {
        scanString();
        return;
    }

    switch (next())
    {
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

    case '&': emit(next('&') ? Token::Type::AndAnd : Token::Type::And); break;
    case '!': emit(next('=') ? Token::Type::BangEqual : Token::Type::Bang); break;
    case '=': emit(next('=') ? Token::Type::EqualEqual : Token::Type::Equal); break;
    case '>': emit(next('=') ? Token::Type::GreaterEqual : Token::Type::Greater); break;
    case '<': emit(next('=') ? Token::Type::LessEqual : Token::Type::Less); break;
    case '|': emit(next('|') ? Token::Type::PipePipe : Token::Type::Pipe); break;
    }
}
