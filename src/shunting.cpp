#include "shunting.h"

using namespace std;

extern Stack<shuntingToken> operatorStack;

int getPrecedence(const wchar_t *operatorToken)
{
    if (wcscmp(operatorToken, L"|") == 0)
    {
        return 1;
    }
    else if (wcscmp(operatorToken, L".") == 0)
    {
        return 2;
    }
    else if (wcscmp(operatorToken, L"*") == 0 || wcscmp(operatorToken, L"+") == 0 || wcscmp(operatorToken, L"?") == 0)
    {
        return 3;
    }
    else if (wcscmp(operatorToken, L"(") == 0 || wcscmp(operatorToken, L")") == 0)
    {
        return 4;
    }
    else
    {
        return 0;
    }
}

const wchar_t *clean(wchar_t *&input)
{
    wstring output = L"";
    for (int i = 0; i < wcslen(input); i++)
    {
        if (input[i] != ' ')
        {
            output += input[i];
        }
    }
    return wcsdup(output.c_str());
}

bool isOperand(const wchar_t *token)
{
    return !wcschr(L"|*+?.()", token[0]);
}

shuntingToken::TokenType getOperatorType(const wchar_t *token)
{
    if (wcscmp(token, L"(") == 0 || wcscmp(token, L")") == 0)
    {
        return shuntingToken::BRACKET;
    }
    else if (wcscmp(token, L"*") == 0 || wcscmp(token, L"+") == 0 || wcscmp(token, L"?") == 0)
    {
        return shuntingToken::UNARY_OPERATOR;
    }
    else
    {
        return shuntingToken::BINARY_OPERATOR;
    }
}

Stack<shuntingToken> getTokens(const wchar_t *&infix)
{
    Stack<shuntingToken> tokens;
    const int infixLength = wcslen(infix);

    for (int i = 0; i < infixLength; i++)
    {

        const wchar_t *actualChar = wstring(1, infix[i]).c_str();
        const wchar_t *rightChar = wstring(1, infix[i + 1]).c_str();
        if (isOperand(actualChar))
        {
            shuntingToken operand;
            operand.token = wcsdup(actualChar);
            operand.precedence = getPrecedence(actualChar);
            operand.type = shuntingToken::OPERAND;
            tokens.push(operand);

            if (wcscmp(rightChar, L"") != 0 && isOperand(rightChar))
            {
                shuntingToken operatorToken;
                operatorToken.token = wcsdup(L".");
                operatorToken.precedence = getPrecedence(L".");
                operatorToken.type = getOperatorType(L".");
                tokens.push(operatorToken);
            }
            else if (wcscmp(rightChar, L"(") == 0)
            {
                shuntingToken operatorToken;
                operatorToken.token = wcsdup(L".");
                operatorToken.precedence = getPrecedence(L".");
                operatorToken.type = getOperatorType(L".");
                tokens.push(operatorToken);
            }
        }
        else
        {
            shuntingToken operatorToken;
            operatorToken.token = wcsdup(actualChar);
            operatorToken.precedence = getPrecedence(actualChar);
            operatorToken.type = getOperatorType(actualChar);
            tokens.push(operatorToken);

            if (operatorToken.type == shuntingToken::UNARY_OPERATOR && (isOperand(rightChar) || wcscmp(rightChar, L"(") == 0))
            {
                shuntingToken concatToken;
                concatToken.token = wcsdup(L".");
                concatToken.precedence = getPrecedence(L".");
                concatToken.type = getOperatorType(L".");
                tokens.push(concatToken);
            }
            else if (wcscmp(operatorToken.token, L")") == 0 && (isOperand(rightChar) || wcscmp(rightChar, L"(") == 0))
            {
                shuntingToken concatToken;
                concatToken.token = wcsdup(L".");
                concatToken.precedence = getPrecedence(L".");
                concatToken.type = getOperatorType(L".");
                tokens.push(concatToken);
            }
        }
    }

    return tokens;
}

wstring expandRanges(const wstring &input)
{
    wstring output;
    for (size_t i = 0; i < input.size(); ++i)
    {
        if (input[i] == L'[')
        {
            size_t end = input.find(L']', i);
            if (end != wstring::npos && end > i + 2 && input[i + 2] == L'-')
            {
                wchar_t startChar = input[i + 1];
                wchar_t endChar = input[i + 3];
                output += L"("; // Add opening parenthesis
                for (wchar_t c = startChar; c <= endChar; ++c)
                {
                    output += c;
                    if (c != endChar)
                    {
                        output += L"|";
                    }
                }
                output += L")"; // Add closing parenthesis
                i = end;
            }
            else
            {
                output += input[i];
            }
        }
        else
        {
            output += input[i];
        }
    }
    return output;
}

Stack<shuntingToken> shuntingYard(const wchar_t *infix)
{
    Stack<shuntingToken> tokens;
    wstring expandedInfix = expandRanges(infix);
    const wchar_t *cleanedInfix = (wchar_t *)clean((wchar_t *&)expandedInfix);
    tokens = getTokens(cleanedInfix);
    /*     const wchar_t *cleanedInfix = (wchar_t *)clean((wchar_t *&)infix);
        tokens = getTokens(cleanedInfix); */
    wcout << "\n\033[1;34m"
          << "Shunting Yard Input"
          << ": \033[0m" << cleanedInfix << endl;

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
        else if (wcscmp(o1.token, L"(") == 0)
        {
            aux.push(o1);
        }
        else if (wcscmp(o1.token, L")") == 0)
        {
            while (!aux.isEmpty() && wcscmp(aux.getTop()->token, L"(") != 0)
            {
                output.push(aux.pop());
            }

            if (!aux.isEmpty() && wcscmp(aux.getTop()->token, L"(") == 0)
            {
                aux.pop();
            }
            else if (aux.isEmpty())
            {
                throw invalid_argument("Mismatched parenthesis");
            }
        }
    }

    for (int i = 0; aux.getSize() > 0; i++)
    {
        if (!aux.isEmpty() && aux.getTop()->type == shuntingToken::BRACKET)
        {
            throw invalid_argument("Mismatched parenthesis");
        }
        else
        {
            output.push(aux.pop());
        }
    }

    output.reverse();

    wcout << "\033[1;34m"
          << "Shunting Yard Output"
          << ": \033[0m";
    for (int i = 0; i < output.getSize(); i++)
    {
        shuntingToken token = output.getTop()[-i];
        wcout << token.token;
    }

    wcout << endl;
    return output;
}
