#include "scanner.h"

#include "error.h"

void Scanner::scan(std::string_view source)
{
    indentation.clear();
    indentation.push(0);

    tokens.clear();
    begin = source.data();
    current = source.data();
    line = 1;
    level = 0;
    line_begin = true;

    while (true)
    {
        tokens.push_back(scanToken());
        if (tokens.back().type == Token::Type::Eof)
            return;
    }
}

void Scanner::skipWhitespace()
{
    constexpr auto kIndentSize = 2;

    while (true)
    {
        switch (peek())
        {
        case ' ':
            if (line_begin && level == 0)
            {
                int spaces = 0;
                while (match(' '))
                    spaces++;

                if (spaces & (kIndentSize - 1))
                    throw SyntaxError("indentation must be a multiple of {}", kIndentSize);

                int indent = spaces / kIndentSize;
                if (indent > indentation.peek())
                {
                    if ((indent - indentation.peek()) > 1)
                        throw SyntaxError("Indentation of multiple levels");

                    indentation.push(indent);
                }

                if (indentation.empty())
                {
                    indentation.push(indent);
                    tokens.push_back(makeToken(Token::Type::Indent));
                }

                int current = indentation.empty() ? 0 : indentation.peek();

                indentation.push(spaces / 2);

                line_begin = false;
            }
            break;
            
        case '\t':
            throw SyntaxError("tabs are not supported");

        case '\r':
            next();
            break;

        case '#':
            skipComment();
            break;

        case '\n':
            line++;
            line_begin = true;
            next();
            break;

        default:
            return;
        }
    }
}

void Scanner::skipEmptyLine()
{
}

void Scanner::skipComment()
{
    while (!finished() && peek() != '\n')
        next();
}

Token Scanner::makeToken(Token::Type type) const
{
    Token token;
    token.type = type;
    token.lexeme = std::string_view(begin, current - begin);
    token.line = line;
    return token;
}

Token Scanner::scanToken()
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

    if (finished())
        return makeToken(Token::Type::Eof);

    auto c = next();

    switch (c)
    {
    // Single
    case '{': return makeToken(Token::Type::BraceLeft);
    case '}': return makeToken(Token::Type::BraceRight);
    case '[': return makeToken(Token::Type::BracketLeft);
    case ']': return makeToken(Token::Type::BracketRight);
    case '^': return makeToken(Token::Type::Caret);
    case ':': return makeToken(Token::Type::Colon);
    case ',': return makeToken(Token::Type::Comma);
    case '.': return makeToken(Token::Type::Dot);
    case '-': return makeToken(Token::Type::Minus);
    case '(': return makeToken(Token::Type::ParenLeft);
    case ')': return makeToken(Token::Type::ParenRight);
    case '%': return makeToken(Token::Type::Percent);
    case '+': return makeToken(Token::Type::Plus);
    case '/': return makeToken(Token::Type::Slash);
    case '*': return makeToken(Token::Type::Star);

    // Single or double
    case '&': return makeToken(match('&') ? Token::Type::AndAnd : Token::Type::And);
    case '!': return makeToken(match('=') ? Token::Type::BangEqual: Token::Type::Bang);
    case '=': return makeToken(match('=') ? Token::Type::EqualEqual: Token::Type::Equal);
    case '>': return makeToken(match('=') ? Token::Type::GreaterEqual: Token::Type::Greater);
    case '<': return makeToken(match('=') ? Token::Type::LessEqual: Token::Type::Less);
    case '|': return makeToken(match('|') ? Token::Type::PipePipe: Token::Type::Pipe);
    }

    // Todo: throw


    return Token();
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
    return finished() ? '\0' : current[1];
}

bool Scanner::finished() const
{
    return *current == '\0';
}

bool Scanner::match(char expected)
{
    if (finished() || *current != expected)
        return false;

    current++;
    return true;
}
