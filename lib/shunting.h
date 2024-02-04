#ifndef SHUNTING_H
#define SHUNTING_H

#include "stack.h"
#include <string.h>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace std;

struct shuntingToken
{
    char *token;
    int precedence;
    enum TokenType
    {
        UNARY_OPERATOR,
        BINARY_OPERATOR,
        OPERAND,
        BRACKET
    };
    TokenType type;
};

int getPrecedence(const char *operatorToken);

const char *clean(char *&input);

bool isOperand(const char *token);

shuntingToken::TokenType getOperatorType(const char *token);

Stack<shuntingToken> getTokens(const char *&infix);

char *shuntingYard(const char *infix);

#endif