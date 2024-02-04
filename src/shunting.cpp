#include "shunting.h"

using namespace std;

Stack<shuntingToken> operatorStack;

int getPrecedence(const char *operatorToken)
{
    if (strcmp(operatorToken, "|") == 0)
    {
        return 1;
    }
    else if (strcmp(operatorToken, ".") == 0 || strcmp(operatorToken, "?") == 0)
    {
        return 2;
    }
    else if (strcmp(operatorToken, "*") == 0 || strcmp(operatorToken, "+") == 0)
    {
        return 3;
    }
    else if (strcmp(operatorToken, "(") == 0 || strcmp(operatorToken, ")") == 0)
    {
        return 4;
    }
    else
    {
        return 0;
    }
}

const char *operators[] = {"+", "-", "*", "/", "^", "(", ")"};

const char *clean(char *&input)
{
    string output = "";
    for (int i = 0; i < strlen(input); i++)
    {
        if (input[i] != ' ')
        {
            output += input[i];
        }
    }
    return strdup(output.c_str());
}

// Determina si un caracter o token es un operador o no
bool isOperand(const char *token)
{
    return !strchr("|*+?.()", token[0]);
}

shuntingToken::TokenType getOperatorType(const char *token)
{
    if (strcmp(token, "(") == 0 || strcmp(token, ")") == 0)
    {
        return shuntingToken::BRACKET;
    }
    else if (strcmp(token, "*") == 0 || strcmp(token, "+") == 0 || strcmp(token, "?") == 0)
    {
        return shuntingToken::UNARY_OPERATOR;
    }
    else
    {
        return shuntingToken::BINARY_OPERATOR;
    }
}

Stack<shuntingToken> getTokens(const char *&infix)
{
    Stack<shuntingToken> tokens;
    const int infixLength = strlen(infix);

    for (int i = 0; i < infixLength; i++)
    {

        const char *actualChar = string(1, infix[i]).c_str();
        const char *rightChar = string(1, infix[i + 1]).c_str();
        if (isOperand(actualChar))
        {
            shuntingToken operand;
            operand.token = strdup(actualChar);
            operand.precedence = getPrecedence(actualChar);
            operand.type = shuntingToken::OPERAND;
            tokens.push(operand);

            if (strcmp(rightChar, "") != 0 && isOperand(rightChar))
            {
                shuntingToken operatorToken;
                operatorToken.token = strdup(".");
                operatorToken.precedence = getPrecedence(".");
                operatorToken.type = getOperatorType(".");
                tokens.push(operatorToken);
            }
            else if (strcmp(rightChar, "(") == 0)
            {
                shuntingToken operatorToken;
                operatorToken.token = strdup(".");
                operatorToken.precedence = getPrecedence(".");
                operatorToken.type = getOperatorType(".");
                tokens.push(operatorToken);
            }
        }
        else
        {
            shuntingToken operatorToken;
            operatorToken.token = strdup(actualChar);
            operatorToken.precedence = getPrecedence(actualChar);
            operatorToken.type = getOperatorType(actualChar);
            tokens.push(operatorToken);

            if (operatorToken.type == shuntingToken::UNARY_OPERATOR && isOperand(rightChar))
            {
                shuntingToken concatToken;
                concatToken.token = strdup(".");
                concatToken.precedence = getPrecedence(".");
                concatToken.type = getOperatorType(".");
                tokens.push(concatToken);
            }
        }
    }

    return tokens;
}

char *shuntingYard(const char *infix)
{
    Stack<shuntingToken> tokens;
    const char *cleanedInfix = (char *)clean((char *&)infix);
    tokens = getTokens(cleanedInfix);
    cout << cleanedInfix << endl;

    Stack<shuntingToken> aux;
    Stack<shuntingToken> output;

    tokens.reverse();

    for (int i = 0; tokens.getSize() > 0; i++)
    {
        shuntingToken o1 = tokens.pop();
        if (o1.type == shuntingToken::OPERAND)
        {
            output.push(o1);
        }
        else if (o1.type != shuntingToken::BRACKET)
        {
            while (!aux.isEmpty() && aux.getTop()->type != shuntingToken::BRACKET)
            {
                if (o1.type == shuntingToken::UNARY_OPERATOR && o1.precedence <= aux.getTop()->precedence)
                {
                    output.push(aux.pop());
                }
                else if (o1.type == shuntingToken::BINARY_OPERATOR && o1.precedence < aux.getTop()->precedence)
                {
                    output.push(aux.pop());
                }
                else
                {
                    break;
                }
            }
            aux.push(o1);
        }
        else if (strcmp(o1.token, "(") == 0)
        {
            aux.push(o1);
        }
        else if (strcmp(o1.token, ")") == 0)
        {
            while (!aux.isEmpty() && strcmp(aux.getTop()->token, "(") != 0)
            {
                output.push(aux.pop());
            }

            if (!aux.isEmpty() && strcmp(aux.getTop()->token, "(") == 0)
            {
                aux.pop();
            }
            else if (aux.isEmpty())
            {
                throw std::invalid_argument("Mismatched parenthesis");
            }
        }
    }

    for (int i = 0; aux.getSize() > 0; i++)
    {
        if (!aux.isEmpty() && aux.getTop()->type == shuntingToken::BRACKET)
        {
            throw std::invalid_argument("Mismatched parenthesis");
        }
        else
        {
            output.push(aux.pop());
        }
    }

    output.reverse();

    for (int i = 0; output.getSize() > 0; i++)
    {
        shuntingToken token = output.pop();
        cout << token.token;
    }

    cout << endl;
    return 0;
}
