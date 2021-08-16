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

    while (!isEof())
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

bool Scanner::isDigit(char c)
{
    return c >= '0' && c <= '9';
}

bool Scanner::isAlpha(char c)
{
    return (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z')
        || (c == '_');
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

bool Scanner::next(std::string_view match)
{
    const char* iter = cursor;
    for (char c : match)
    {
        if (*iter == '\0' || *iter != c)
            return false;
        iter++;
    }
    
    cursor += match.size();
    return true;
}

char Scanner::peek() const
{
    return *cursor;
}

char Scanner::peek(std::size_t index) const
{
    const char* iter = cursor;
    while (*iter && index--)
        iter++;

    return *iter;
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

    //// Todo: remove when functions are implemented
    //Print

    char c = next();

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

void Scanner::scanString()
{
    while (peek() != '"' && !isEof())
    {
        if (peek() == '\n')
            line++;

        next();
    }

    if (isEof())
        throw SyntaxError("unterminated string");

    next();

    emit(Token::Type::String);
}

void Scanner::scanNumber()
{
    auto scan_digits = [this]()
    {
        while (isDigit(peek()))
            next();
    };

    bool is_bin = std::string_view(cursor - 1, 2) == "0b";
    bool is_hex = std::string_view(cursor - 1, 2) == "0x";
    if ((is_bin || is_hex))
    {
        if (!isDigit(peek(1)))
            throw SyntaxError("invalid {} literal", is_bin ? "binary" : "hexadecimal");

        next();
        scan_digits();
        emit(Token::Type::Integer);
    }
    else
    {
        bool integer = true;
        if (peek() == '.' && isDigit(peek(1)))
        {
            next();
            integer = false;
            scan_digits();
        }

        emit(integer ? Token::Type::Integer : Token::Type::Float);
    }
}
