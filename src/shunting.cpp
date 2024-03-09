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
    // Si el token es un caracter escapado, trátalo como un operando
    if (token[0] == L'\\' && wcschr(L"|*+?.()", token[1]))
    {
        return true;
    }
    else if (wcschr(L"|*+?.()", token[0]) && token[-1] == L'\\')
    {
        return true;
    }
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
    bool isEscaped = false;

    for (int i = 0; i < infixLength; i++)
    {

        const wchar_t *actualChar = wstring(1, infix[i]).c_str();
        const wchar_t *rightChar = wstring(1, infix[i + 1]).c_str();
        const wchar_t *rightMostChar = wstring(1, infix[i + 2]).c_str();
        if (actualChar[0] == L'\\' && !isEscaped)
        {
            isEscaped = true;
            continue;
        }

        if (isEscaped)
        {
            shuntingToken operand;
            operand.token = wcsdup((wstring(actualChar)).c_str());
            operand.precedence = getPrecedence(actualChar);
            operand.type = shuntingToken::OPERAND;
            tokens.push(operand);
            isEscaped = false;

            if (wcscmp(rightChar, L"") != 0 && isOperand(rightChar))
            {
                shuntingToken operatorToken;
                operatorToken.token = wcsdup(L".");
                operatorToken.precedence = getPrecedence(L".");
                operatorToken.type = getOperatorType(L".");
                tokens.push(operatorToken);
            }
            else if (wcscmp(rightChar, L"(") == 0 && wcscmp(rightMostChar, L")") != 0)
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
                else if (wcscmp(rightChar, L"(") == 0 && wcscmp(rightMostChar, L")") != 0)
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

                /* if (operatorToken.type == shuntingToken::UNARY_OPERATOR &&
                    (i == 0 || (!isOperand(wstring(1, infix[i - 1]).c_str()) && infix[i - 2] != L'\\')))
                {
                    wcout << infix[i - 1] << endl;
                    std::string errorMsg = "Error de sintaxis: Operador unario '";
                    errorMsg += std::string(actualChar, actualChar + wcslen(actualChar)); // Convertir wchar_t a std::string
                    errorMsg += "' sin operando a su izquierda. Si quieres usar el operador como un literal, escápalo con una barra invertida '\\'";
                    throw std::invalid_argument(errorMsg);
                }
                else if (operatorToken.type == shuntingToken::BINARY_OPERATOR &&
                         (i == 0 || (!isOperand(wstring(1, infix[i - 1]).c_str()) && infix[i - 2] != L'\\') ||
                          i == infixLength - 1 || !isOperand(wstring(1, infix[i + 1]).c_str())))
                {
                    std::string errorMsg = "Error de sintaxis: Operador binario '";
                    errorMsg += std::string(actualChar, actualChar + wcslen(actualChar)); // Convertir wchar_t a std::string
                    errorMsg += "' sin operando a su izquierda o derecha. Si quieres usar el operador como un literal, escápalo con una barra invertida '\\'";
                    throw std::invalid_argument(errorMsg);
                }

                else */
                if (operatorToken.type == shuntingToken::UNARY_OPERATOR && (isOperand(rightChar) || (wcscmp(rightChar, L"(") == 0 && wcscmp(rightMostChar, L")") != 0)))
                {
                    shuntingToken concatToken;
                    concatToken.token = wcsdup(L".");
                    concatToken.precedence = getPrecedence(L".");
                    concatToken.type = getOperatorType(L".");
                    tokens.push(concatToken);
                }
                else if (wcscmp(operatorToken.token, L")") == 0 && (isOperand(rightChar) || (wcscmp(rightChar, L"(") == 0 && wcscmp(rightMostChar, L")") != 0)))
                {
                    shuntingToken concatToken;
                    concatToken.token = wcsdup(L".");
                    concatToken.precedence = getPrecedence(L".");
                    concatToken.type = getOperatorType(L".");
                    tokens.push(concatToken);
                }
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
            bool openingBracketEscaped = (i > 0 && input[i - 1] == L'\\');
            size_t end = input.find(L']', i + 1);
            if (end != wstring::npos)
            {
                bool closingBracketEscaped = (end > 0 && input[end - 1] == L'\\');
                if (openingBracketEscaped != closingBracketEscaped)
                {
                    throw std::invalid_argument("Syntax Regexp Error: Unmatched escaped bracket. Cannot mix escaped and unescaped brackets in the same character class");
                }
                else if (!openingBracketEscaped && end > i + 2 && input[i + 2] == L'-')
                {
                    wchar_t startChar = input[i + 1];
                    wchar_t endChar = input[i + 3];
                    output += L"(";
                    for (wchar_t c = startChar; c <= endChar; ++c)
                    {
                        output += c;
                        if (c != endChar)
                        {
                            output += L"|";
                        }
                    }
                    output += L")";
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

    for (int i = 0; i < tokens.getSize(); i++)
    {
        /*  wcout << L"Token: " << tokens.getTop()[-i].token << L" Type: " << tokens.getTop()[-i].type << endl;
         wcout << L"Size: " << tokens.getSize() << endl; */
        if (i == 0 && tokens.getTop()[-i].type == shuntingToken::UNARY_OPERATOR)
        {
            throw invalid_argument("Regexp Syntax Error: Unary operator at the beginning of the expression");
        }
        else if (i == 0 && tokens.getTop()[-i].type == shuntingToken::BINARY_OPERATOR)
        {
            throw invalid_argument("Regexp Syntax Error: Binary operator at the beginning of the expression");
        }
        else if (tokens.getTop()[-i].type == shuntingToken::BINARY_OPERATOR)
        {
            if (i - 1 < 0 || i + 1 > tokens.getSize())
            {
                throw invalid_argument("Regexp Syntax Error: Binary operator at the end of the expression");
            }
            else if (tokens.getTop()[-i - 1].type == shuntingToken::BINARY_OPERATOR)
            {
                throw invalid_argument("Regexp Syntax Error: Binary operator followed by another binary operator");
            }
            else if (tokens.getTop()[-i + 1].type == shuntingToken::BINARY_OPERATOR)
            {
                throw invalid_argument("Regexp Syntax Error: Binary operator followed by another binary operator");
            }
        }
        else if (tokens.getTop()[-i].type == shuntingToken::UNARY_OPERATOR)
        {
            bool operandFound = false;
            for (int j = tokens.getSize() - 2; j >= 0; j--)
            {
                if (tokens.getTop()[-j].type == shuntingToken::OPERAND)
                {
                    operandFound = true;
                    break;
                }
            }

            if (!operandFound)
            {
                std::string errorMsg = "Regexp Syntax Error: Mismatched operators. Unary operator '";
                errorMsg += std::string(tokens.getTop()[-i].token, tokens.getTop()[-i].token + wcslen(tokens.getTop()[-i].token)); // Convertir wchar_t a std::string
                errorMsg += "' without operand";
                throw std::invalid_argument(errorMsg);
            }
        }
        else if (tokens.getTop()[-i].type == shuntingToken::BRACKET)
        {
            if (i + 1 > tokens.getSize() && wcscmp(tokens.getTop()[-1].token, L"(") == 0)
            {
                throw invalid_argument("Regexp Syntax Error: Mismatched parenthesis");
            }
            else if (i + 1 < tokens.getSize() && tokens.getTop()[-i - 1].type == shuntingToken::BINARY_OPERATOR && wcscmp(tokens.getTop()[-i].token, L"(") == 0)
            {
                throw invalid_argument("Regexp Syntax Error: Opening parenthesis before binary operator");
            }
            else if (i - 1 > 0 && tokens.getTop()[-i + 1].type == shuntingToken::BINARY_OPERATOR && wcscmp(tokens.getTop()[-i].token, L")") == 0)
            {
                throw invalid_argument("Regexp Syntax Error: Closing parenthesis after binary operator");
            }
        }
    }

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
