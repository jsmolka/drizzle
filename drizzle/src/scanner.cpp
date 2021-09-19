#include "scanner.h"

#include <tuple>

#include "errors.h"

Tokens Scanner::scan(const std::string& source)
{
    cursor = source.data();
    lexeme = source.data();
    line = 0;
    indentation = 0;
    tokens.clear();

    scanBlankLines();
    scanIndentation();

    while (*cursor)
    {
        lexeme = cursor;
        scanToken();
        scanWhitespace();
    }

    emit(Token::Type::Eof);

    return tokens;
}

template<char Base>
bool Scanner::isDigit(char c)
{
    static_assert(Base <= 36);

    if constexpr (Base <= 10)
        return (c >= '0') && (c < ('0' + Base));

    if constexpr (Base <= 36)
        return (c >= '0' && c <= '9')
            || (c >= 'a' && c < ('a' + Base - 10))
            || (c >= 'A' && c < ('A' + Base - 10));
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
    token.line = line;
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
                throw SyntaxError(begin, "tabs used for indent");

            default:
                return spaces;
            }
        }
    };

    int spaces = count_spaces();
    if (spaces % kSpacesPerIndentation)
        throw SyntaxError(begin, "indent spaces must be a multiple of {}", kSpacesPerIndentation);

    int indent = spaces / kSpacesPerIndentation;
    if (indent > indentation)
    {
        if ((indent - indentation) > 1)
            throw SyntaxError(begin, "too many indents at once");

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
            emit(Token::Type::NewLine);
            line++;
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
            
            if (next() == '\n')
                line++;
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
                throw SyntaxError(cursor, "unexpected line break");

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
                    throw SyntaxError(cursor, "unknown escape sequence");
                }
                break;

            default:
                next();
                break;
            }
        }
    }

    if (begin)
        throw SyntaxError(begin, "unterminated string");

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
            throw SyntaxError(cursor, "expected bin digit");

        while (is_bin(*cursor))
            next();

        emit(Token::Type::Integer);
        parseInt(2);
        return;
    }
    
    if (next("0x"))
    {
        if (!is_hex(*cursor))
            throw SyntaxError(cursor, "expected hex digit");

        while (is_hex(*cursor))
            next();

        emit(Token::Type::Integer);
        parseInt(16);
        return;
    }

    if (*cursor == '0' && is_dec(peek()))
        throw SyntaxError(cursor, "unexpected zero");

    while (is_dec(*cursor))
        next();

    if (next('.'))
    {
        if (!is_dec(*cursor))
            throw SyntaxError(cursor, "expected digit");

        while (is_dec(*cursor))
            next();

        emit(Token::Type::Float);
        parseFloat();
    }
    else
    {
        emit(Token::Type::Integer);
        parseInt(10);
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

    for (const auto& [keyword, token] : kKeywords)
    {
        if (identifier == keyword)
        {
            emit(token);
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
    case '*': emit(Token::Type::Star); break;
    case '~': emit(Token::Type::Tilde); break;

    case '&': emit(next('&') ? Token::Type::And2 : Token::Type::And); break;
    case '!': emit(next('=') ? Token::Type::BangEqual : Token::Type::Bang); break;
    case '=': emit(next('=') ? Token::Type::Equal2 : Token::Type::Equal); break;
    case '|': emit(next('|') ? Token::Type::Pipe2 : Token::Type::Pipe); break;
    case '/': emit(next('/') ? Token::Type::Slash2 : Token::Type::Slash); break;

    case '>':
        if (next('>'))
            emit(next('>') ? Token::Type::Greater3 : Token::Type::Greater2);
        else
            emit(next('=') ? Token::Type::GreaterEqual : Token::Type::Greater);
        break;

    case '<': 
        if (next('<'))
            emit(Token::Type::Less2);
        else if (next('='))
            emit(Token::Type::LessEqual);
        else
            emit(Token::Type::Less);
        break;

    default:
        throw SyntaxError(cursor - 1, "unexpected character");
    }
}

void Scanner::parseInt(int base)
{
    SHELL_ASSERT(base == 2 || base == 10 || base == 16);

    auto& token = tokens.back();
    std::string_view lexeme = token.lexeme;
    if (base == 2 || base == 16)
        lexeme.remove_prefix(2);

    dzint value = std::strtoull(lexeme.data(), nullptr, base);
    if (errno == ERANGE)
    {
        errno = 0;
        throw SyntaxError(token.lexeme.data(), "cannot parse int");
    }
    token.value.set(value);
}

void Scanner::parseFloat()
{
    auto& token = tokens.back();

    dzfloat value = std::strtod(token.lexeme.data(), nullptr);
    if (errno == ERANGE)
    {
        errno = 0;
        throw SyntaxError(token.lexeme.data(), "cannot parse float");
    }
    token.value.set(value);
}
