#ifndef SHUNTING_H
#define SHUNTING_H

#include "stack.h"
#include <string.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <locale>
#include <codecvt>

using namespace std;

struct shuntingToken
{
    wchar_t *token;
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

int getPrecedence(const wchar_t *operatorToken);

const char *clean(char *&input);

bool isOperand(const wchar_t *token);

shuntingToken::TokenType getOperatorType(const wchar_t *token);

Stack<shuntingToken> getTokens(const wchar_t *&infix);

Stack<shuntingToken> shuntingYard(const wchar_t *infix);

#endif