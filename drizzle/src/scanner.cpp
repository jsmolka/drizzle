#include "scanner.h"

#include "error.h"

std::vector<Token> Scanner::scan(const std::string& source)
{
    begin = source.data();
    current = source.data();
    line = 1;
    indentation = 0;

    tokens.clear();
    while (!isFileEnd())
    {
        scanWhitespace();
        scanToken();
    }
    token(Token::Type::Eof);

    return tokens;
}

bool Scanner::isFileEnd() const
{
    return *current == '\0';
}

bool Scanner::isLineBegin() const
{
    return tokens.empty() || tokens.back().type == Token::Type::NewLine;
}

void Scanner::skipEmptyLines()
{
    while (!isFileEnd())
    {
        auto previous = current;
        auto line_end = false;

        while (!isFileEnd() && !line_end)
        {
            switch (peek())
            {
            case ' ':
            case '\t':
            case '\r':
                next();
                break;

            case '#':
                skipComment();
                break;

            case '\n':
                line++;
                line_end = true;
                next();
                break;

            default:
                current = previous;
                return;
            }
        }
    }
}

void Scanner::skipComment()
{
    while (!isFileEnd() && peek() != '\n')
        next();
}

void Scanner::token(Token::Type type)
{
    Token token;
    token.type = type;
    token.lexeme = std::string_view(begin, current - begin);
    token.line = line;
    tokens.push_back(token);

    begin = current;
}

void Scanner::scanWhitespace()
{
    while (true)
    {
        if (isLineBegin())
            skipEmptyLines();

        switch (peek())
        {
        case ' ':
            if (isLineBegin())
                scanIndentation();
            else
                next();
            break;
            
        case '\t':
            if (isLineBegin())
                throw SyntaxError("tabs cannot be used for indentation");
            else
                next();
            break;

        case '\r':
            next();
            break;

        case '#':
            skipComment();
            break;

        case '\n':
            line++;
            next();
            token(Token::Type::NewLine);
            break;

        default:
            if (isLineBegin() && indentation > 0)
            {
                while (indentation--)
                    token(Token::Type::Dedent);
            }
            return;
        }
    }
}

void Scanner::scanIndentation()
{
    constexpr auto kIndentationSize = 2;

    int spaces = 0;
    while (match(' '))
        spaces++;

    if (spaces & (kIndentationSize - 1))
        throw SyntaxError("indentation must be a multiple of {}", kIndentationSize);

    int indent = spaces / kIndentationSize;
    if (indent > indentation)
    {
        if ((indent - indentation) > 1)
            throw SyntaxError("invalid indentation");

        token(Token::Type::Indent);
    }
    else
    {
        while (indent < indentation)
        {
            token(Token::Type::Dedent);
            indentation--;
        }
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

        //// Special
        //Eof, Indent,

        //// Todo: remove when functions are implemented
        //Print

    switch (next())
    {
    // Single
    case '{': return token(Token::Type::BraceLeft);
    case '}': return token(Token::Type::BraceRight);
    case '[': return token(Token::Type::BracketLeft);
    case ']': return token(Token::Type::BracketRight);
    case '^': return token(Token::Type::Caret);
    case ':': return token(Token::Type::Colon);
    case ',': return token(Token::Type::Comma);
    case '.': return token(Token::Type::Dot);
    case '-': return token(Token::Type::Minus);
    case '(': return token(Token::Type::ParenLeft);
    case ')': return token(Token::Type::ParenRight);
    case '%': return token(Token::Type::Percent);
    case '+': return token(Token::Type::Plus);
    case '/': return token(Token::Type::Slash);
    case '*': return token(Token::Type::Star);

    // Single or double
    case '&': return token(match('&') ? Token::Type::AndAnd : Token::Type::And);
    case '!': return token(match('=') ? Token::Type::BangEqual : Token::Type::Bang);
    case '=': return token(match('=') ? Token::Type::EqualEqual : Token::Type::Equal);
    case '>': return token(match('=') ? Token::Type::GreaterEqual : Token::Type::Greater);
    case '<': return token(match('=') ? Token::Type::LessEqual : Token::Type::Less);
    case '|': return token(match('|') ? Token::Type::PipePipe : Token::Type::Pipe);
    }

    // Todo: throw
}

char Scanner::next()
{
    current++;
    return current[-1];
}

char Scanner::peek() const
{
    return *current;
}

char Scanner::peekNext() const
{
    return isFileEnd() ? '\0' : current[1];
}

bool Scanner::match(char expected)
{
    if (isFileEnd() || *current != expected)
        return false;

    current++;
    return true;
}
