#pragma once

#include "token.h"  // Todo: move into ast/
#include "ast/expression.h"

class Parser
{
public:
    std::unique_ptr<Expression> parse(const Tokens& tokens);

private:
    Tokens::const_iterator current;
    Tokens::const_iterator previous;
};
