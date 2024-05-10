
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <map>

template <typename T>
class Stack
{
private:
    T *top;
    int size;
    std::vector<T> elements;

public:
    Stack() : top(nullptr), size(0) {}
    void push(T const &value);
    bool isEmpty();
    T pop();
    T *getTop();
    T last();
    int getSize();
    void reverse();
};

template <typename T>
void Stack<T>::push(T const &value)
{
    elements.push_back(value);
    size++;
    top = &elements[size - 1];
}

template <typename T>
bool Stack<T>::isEmpty()
{
    return size == 0;
}

template <typename T>
T Stack<T>::pop()
{
    if (isEmpty())
    {
        throw std::out_of_range("Mismatched operators");
    }
    else
    {
        size--;
        T value = elements[size];
        elements.pop_back();
        if (size > 0)
        {
            top = &elements[size - 1];
        }
        else
        {
            top = nullptr;
        }
        return value;
    }
}

template <typename T>
T *Stack<T>::getTop()
{
    return top;
}

template <typename T>
T Stack<T>::last()
{
    return elements[size - 1];
}

template <typename T>
int Stack<T>::getSize()
{
    return size;
}

template <typename T>
void Stack<T>::reverse()
{
    std::reverse(elements.begin(), elements.end());
}

#include <string.h>
#include <iostream>
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
    /* const wchar_t *cleanedInfix = (wchar_t *)clean((wchar_t *&)expandedInfix); */
    const wchar_t *cleanedInfix = (wchar_t *)expandedInfix.c_str();
    tokens = getTokens(cleanedInfix);
    /*     const wchar_t *cleanedInfix = (wchar_t *)clean((wchar_t *&)infix);
        tokens = getTokens(cleanedInfix); */
    /* wcout << "\n\033[1;34m"
          << "Shunting Yard Input"
          << ": \033[0m" << cleanedInfix << endl; */

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

    /* wcout << "\033[1;34m"
          << "Shunting Yard Output"
          << ": \033[0m";
    for (int i = 0; i < output.getSize(); i++)
    {
        shuntingToken token = output.getTop()[-i];
        wcout << token.token;
    }

    wcout << endl; */
    return output;
}

#include <iostream>
#include <vector>

using namespace std;

struct AutomataTransition;
struct AutomataState;
struct Automata;

struct AutomataState
{
    wchar_t *name;
    vector<AutomataTransition *> transitions;
    bool isAcceptable;
    wstring returnType;
};

struct AutomataTransition
{
    AutomataState *from;
    AutomataState *to;
    wchar_t *input;
};

struct Automata
{
    AutomataState *start;
    vector<AutomataState *> states;
    vector<AutomataState *> finalStates;
    wstring alphabet;
    vector<AutomataTransition *> transitions;
};

wstring getAlphabet(Stack<shuntingToken> *postfix);
void printAutomata(Automata *automata);
void completeAFD(Automata *automata);
Automata *deepCopyAutomata(const Automata *original);
Automata *addReturnType(Automata *automata, wstring returnType);
Automata *joinAutomatas(vector<Automata *> automatas);

#include <cwchar>

wstring getAlphabet(Stack<shuntingToken> *postfix)

{
    wstring alphabet = L"";
    for (int i = 0; i < postfix->getSize(); i++)
    {
        shuntingToken token = postfix->getTop()[-i];
        if (token.type == shuntingToken::OPERAND)
        {
            if (!wcschr(alphabet.c_str(), token.token[0]))
            {
                alphabet += token.token;
            }
        }
    }
    return alphabet;
}

void printAutomata(Automata *automata)
{
    if (automata != nullptr)
    {
        wcout << "\n\033[1;36mStart state: \033[0m" << automata->start->name << endl;
        wcout << "\n\033[1;36mFinal states: \033[0m";
        for (int i = 0; i < automata->finalStates.size(); i++)
        {
            wcout << automata->finalStates[i]->name << " ";
        }
        wcout << endl;
        wcout << "\n\033[1;36mTransitions: \033[0m" << endl;
        for (int i = 0; i < automata->transitions.size(); i++)
        {
            wcout << "\033[1;37m" << automata->transitions[i]->from->name << "\033[0;37m -- "
                  << "\033[1;37m" << automata->transitions[i]->input << "\033[0;37m"
                  << " --> "
                  << "\033[1;37m" << automata->transitions[i]->to->name << "\033[0m" << endl;
        }
        wcout << "\n\033[1;36mAlphabet: \033[0m" << automata->alphabet << endl;
        for (int i = 0; i < automata->finalStates.size(); i++)
        {
            wcout << "\n\033[1;36mReturn type: \033[0m" << automata->finalStates[i]->returnType << endl;
        }
    }
}

void completeAFD(Automata *automata)
{
    AutomataState *deadState = new AutomataState();
    deadState->name = L"qDead";
    deadState->isAcceptable = false;

    vector<AutomataTransition *> *transitions = new vector<AutomataTransition *>();
    wstring alphabet = automata->alphabet;

    for (auto transition : automata->transitions)
    {
        transitions->push_back(transition);
    }

    for (auto state : automata->states)
    {
        for (wchar_t symbol : alphabet)
        {
            auto it = std::find_if(transitions->begin(), transitions->end(),
                                   [state, symbol](const auto &transition)
                                   { return wcscmp(transition->from->name, state->name) == 0 && wcscmp(transition->input, wstring(1, symbol).c_str()) == 0; });

            if (it == transitions->end())
            {
                // No transition found, add a new one to the dead state
                AutomataTransition *newTransition = new AutomataTransition();
                newTransition->from = state;
                newTransition->input = wcsdup(wstring(1, symbol).c_str());
                newTransition->to = deadState;
                transitions->push_back(newTransition);
            }
        }
    }

    // Add transitions from the dead state to itself for each letter in the alphabet

    if (automata->transitions.size() < transitions->size())
    {
        for (wchar_t symbol : alphabet)
        {
            AutomataTransition *newTransition = new AutomataTransition();
            newTransition->from = deadState;
            newTransition->input = wcsdup(wstring(1, symbol).c_str());
            newTransition->to = deadState;
            transitions->push_back(newTransition);
        }
        automata->transitions = *transitions;
        automata->states.push_back(deadState);
    }
}

Automata *deepCopyAutomata(const Automata *original)
{
    Automata *newAutomata = new Automata();
    map<AutomataState *, AutomataState *> stateMap;

    // Copy states
    for (AutomataState *state : original->states)
    {
        AutomataState *newState = new AutomataState();
        newState->name = new wchar_t[wcslen(state->name) + 1];
        wcscpy(newState->name, state->name);
        newState->isAcceptable = state->isAcceptable;
        newAutomata->states.push_back(newState);
        stateMap[state] = newState; // Save old to new state mapping
    }

    // Copy transitions
    for (AutomataTransition *transition : original->transitions)
    {
        AutomataTransition *newTransition = new AutomataTransition();
        newTransition->from = stateMap[transition->from];
        newTransition->to = stateMap[transition->to];
        newTransition->input = new wchar_t[wcslen(transition->input) + 1];
        wcscpy(newTransition->input, transition->input);
        newTransition->from->transitions.push_back(newTransition);
        newAutomata->transitions.push_back(newTransition);
    }

    // Copy start state
    newAutomata->start = stateMap[original->start];

    // Copy final states
    for (AutomataState *state : original->finalStates)
    {
        newAutomata->finalStates.push_back(stateMap[state]);
    }

    // Copy alphabet
    newAutomata->alphabet = original->alphabet;

    return newAutomata;
}

Automata *addReturnType(Automata *automata, wstring returnType)
{
    for (int i = 0; i < automata->finalStates.size(); i++)
    {
        automata->finalStates[i]->returnType = returnType;
    }
    return automata;
}

#include <iostream>
#include <set>
using namespace std;

struct TreeNode;

struct TreeNode
{
    shuntingToken *value;
    TreeNode *left;
    TreeNode *right;
    wstring tag = L"";
    bool anulable;
    set<wstring> *firstPos;
    set<wstring> *lastPos;
    set<wstring> *nextPos;
};

TreeNode *createNode(shuntingToken *value);
void deleteNode(TreeNode *node);
TreeNode *constructSyntaxTree(Stack<shuntingToken> *postfix);
void print2DUtil(TreeNode *root, int space);
void print2D(TreeNode *root);
TreeNode *parseTree(TreeNode *root);

TreeNode *createNode(shuntingToken *value)
{
    TreeNode *node = new TreeNode;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void deleteNode(TreeNode *node)
{
    if (node->left != NULL)
    {
        deleteNode(node->left);
    }
    if (node->right != NULL)
    {
        deleteNode(node->right);
    }
    delete node;
    return;
}

TreeNode *constructSyntaxTree(Stack<shuntingToken> *postfix)
{
    Stack<TreeNode *> treeStack;

    for (int i = 0; i < postfix->getSize(); i++)
    {
        shuntingToken token = postfix->getTop()[-i];
        shuntingToken *tokenCopy = new shuntingToken(token);

        if (token.type == shuntingToken::OPERAND)
        {
            TreeNode *node = createNode(tokenCopy);
            treeStack.push(node);
        }
        else
        {
            TreeNode *node = createNode(tokenCopy);
            if (token.type == shuntingToken::UNARY_OPERATOR)
            {
                node->left = treeStack.pop();
            }
            else
            {
                node->right = treeStack.pop();
                node->left = treeStack.pop();
            }
            treeStack.push(node);
        }
    }

    return treeStack.pop();
}

void print2DUtil(TreeNode *root, int space)
{
    const int COUNT = 10;
    if (root == NULL)
        return;

    space += COUNT;

    print2DUtil(root->right, space);

    wcout << endl;
    for (int i = COUNT; i < space; i++)
        wcout << L" ";

    if (wcscmp(root->value->token, L"") != 0)
    {
        wcout << L"\033[1;31m" << root->tag << L" \033[0m";
    }
    wcout << root->value->token << L"\n";

    print2DUtil(root->left, space);
}

void print2D(TreeNode *root)
{
    print2DUtil(root, 0);
}

TreeNode *parseTree(TreeNode *root)
{
    if (root == nullptr)
    {
        return nullptr;
    }

    shuntingToken *newToken = new shuntingToken;
    *newToken = *root->value;
    TreeNode *newRoot = createNode(newToken);
    newRoot->left = parseTree(root->left);
    newRoot->right = parseTree(root->right);

    if (wcscmp(newRoot->value->token, L"?") == 0 && newRoot->value->type == shuntingToken::UNARY_OPERATOR)
    {
        newRoot->value->token = wcsdup(L"|");
        newRoot->value->type = shuntingToken::BINARY_OPERATOR;
        newRoot->value->precedence = 1;
        shuntingToken *epsilonToken = new shuntingToken;
        epsilonToken->token = wcsdup(L"ε");
        epsilonToken->precedence = 0;
        epsilonToken->type = shuntingToken::OPERAND;
        newRoot->right = createNode(epsilonToken);
    }
    else if (wcscmp(newRoot->value->token, L"+") == 0 && newRoot->value->type == shuntingToken::UNARY_OPERATOR)
    {
        newRoot->value->token = wcsdup(L".");
        newRoot->value->type = shuntingToken::BINARY_OPERATOR;
        newRoot->value->precedence = 2;
        shuntingToken *starToken = new shuntingToken;
        starToken->token = wcsdup(L"*");
        starToken->precedence = 3;
        starToken->type = shuntingToken::UNARY_OPERATOR;
        TreeNode *starNode = createNode(starToken);
        starNode->left = parseTree(newRoot->left); // Create a copy of newRoot->left
        newRoot->right = starNode;
    }

    return newRoot;
}

Automata *buildThompsonSnippet(TreeNode *node, wstring &alphabet, vector<Automata *> *automataList);
Automata *thompson(TreeNode *node, wstring &alphabet);

static int counter = 0;

wstring getHashName()
{
    return L"q" + to_wstring(counter++);
}

void addEpsilon(wstring &alphabet)
{
    if (!wcschr(alphabet.c_str(), L'ε'))
    {
        alphabet += L'ε';
    }
}

Automata *buildThompsonSnippet(TreeNode *node, wstring &alphabet, vector<Automata *> *automataList)
{
    Automata *automata = new Automata;

    vector<AutomataState *> *states = new vector<AutomataState *>;
    automata->states = *states;

    vector<AutomataState *> *finalStates = new vector<AutomataState *>;
    automata->finalStates = *finalStates;

    vector<AutomataTransition *> *transitions = new vector<AutomataTransition *>;
    automata->transitions = *transitions;

    automata->alphabet = alphabet;

    if (node->value->type == shuntingToken::OPERAND)
    {
        AutomataState *start = new AutomataState;
        vector<AutomataTransition *> *startTransitions = new vector<AutomataTransition *>;
        start->isAcceptable = false;
        start->name = wcsdup(getHashName().c_str());

        AutomataState *end = new AutomataState;
        vector<AutomataTransition *> *endTransitions = new vector<AutomataTransition *>;
        end->isAcceptable = true;
        end->name = wcsdup(getHashName().c_str());

        AutomataTransition *transition = new AutomataTransition;
        transition->from = start;
        transition->to = end;
        transition->input = node->value->token;

        startTransitions->push_back(transition);
        start->transitions = *startTransitions;
        end->transitions = *endTransitions;

        automata->start = start;
        automata->states.push_back(start);
        automata->states.push_back(end);
        automata->transitions.push_back(transition);
        automata->finalStates.push_back(end);
    }
    else
    {
        if (wcscmp(node->value->token, L".") == 0)
        {
            Automata *right = automataList->back();
            automataList->pop_back();
            Automata *left = automataList->back();
            automataList->pop_back();

            for (int i = 0; i < left->finalStates.size(); i++)
            {
                left->finalStates[i]->isAcceptable = false;
                AutomataTransition *transition = new AutomataTransition;
                transition->from = left->finalStates[i];
                transition->to = right->start;
                transition->input = L"ε";
                left->finalStates[i]->transitions.push_back(transition);
                left->transitions.push_back(transition);
            }

            left->finalStates = right->finalStates;
            left->states.insert(left->states.end(), right->states.begin(), right->states.end());
            left->transitions.insert(left->transitions.end(), right->transitions.begin(), right->transitions.end());

            for (int i = 0; i < left->states.size(); i++)
            {
                for (int j = 0; j < left->states[i]->transitions.size(); j++)
                {
                    if (left->states[i]->transitions[j]->to == left->states[i])
                    {
                        left->states[i]->transitions[j]->to = right->start;
                    }
                }
            }

            for (int i = 0; i < left->finalStates.size(); i++)
            {
                for (int j = 0; j < left->finalStates[i]->transitions.size(); j++)
                {
                    if (left->finalStates[i]->transitions[j]->to == left->finalStates[i])
                    {
                        left->finalStates[i]->transitions[j]->to = right->start;
                    }
                }
            }

            automata->finalStates = left->finalStates;
            automata->start = left->start;
            automata->states = left->states;
            automata->transitions = left->transitions;
        }
        else if (wcscmp(node->value->token, L"|") == 0)
        {
            Automata *right = automataList->back();
            automataList->pop_back();
            Automata *left = automataList->back();
            automataList->pop_back();

            AutomataState *start = new AutomataState;
            start->isAcceptable = false;
            start->name = wcsdup(getHashName().c_str());

            AutomataState *end = new AutomataState;
            end->isAcceptable = true;
            end->name = wcsdup(getHashName().c_str());

            AutomataTransition *toSRight = new AutomataTransition;
            AutomataTransition *toSLeft = new AutomataTransition;

            toSRight->from = start;
            toSRight->to = right->start;
            toSRight->input = L"ε";
            start->transitions.push_back(toSRight);

            toSLeft->from = start;
            toSLeft->to = left->start;
            toSLeft->input = L"ε";
            start->transitions.push_back(toSLeft);

            for (int i = 0; i < right->finalStates.size(); i++)
            {
                AutomataTransition *toERight = new AutomataTransition;
                right->finalStates[i]->isAcceptable = false;
                toERight->from = right->finalStates[i];
                toERight->to = end;
                toERight->input = L"ε";
                right->finalStates[i]->transitions.push_back(toERight);
                left->transitions.push_back(toERight);
            }

            for (int i = 0; i < left->finalStates.size(); i++)
            {
                AutomataTransition *toELeft = new AutomataTransition;
                left->finalStates[i]->isAcceptable = false;
                toELeft->from = left->finalStates[i];
                toELeft->to = end;
                toELeft->input = L"ε";
                left->finalStates[i]->transitions.push_back(toELeft);
                left->transitions.push_back(toELeft);
            }

            left->states.insert(left->states.end(), right->states.begin(), right->states.end());
            left->transitions.insert(left->transitions.end(), right->transitions.begin(), right->transitions.end());

            automata->start = start;
            automata->states = left->states;
            automata->transitions = left->transitions;
            automata->transitions.push_back(toSRight);
            automata->transitions.push_back(toSLeft);
            addEpsilon(alphabet);
            automata->alphabet = alphabet;
            automata->states.push_back(start);
            automata->states.push_back(end);
            automata->finalStates.push_back(end);
        }
        else if (wcscmp(node->value->token, L"*") == 0 || wcscmp(node->value->token, L"+") == 0)
        {

            Automata *left = automataList->back();
            automataList->pop_back();
            automata->states.insert(automata->states.end(), left->states.begin(), left->states.end());
            automata->transitions.insert(automata->transitions.end(), left->transitions.begin(), left->transitions.end());

            AutomataState *start = new AutomataState;
            start->isAcceptable = false;
            start->name = wcsdup(getHashName().c_str());
            automata->start = start;

            AutomataState *end = new AutomataState;
            end->isAcceptable = true;
            end->name = wcsdup(getHashName().c_str());

            AutomataTransition *toS = new AutomataTransition;
            toS->from = start;
            toS->to = left->start;
            toS->input = L"ε";
            start->transitions.push_back(toS);
            automata->transitions.push_back(toS);

            for (int i = 0; i < left->finalStates.size(); i++)
            {
                left->finalStates[i]->isAcceptable = false;
                AutomataTransition *toE = new AutomataTransition;
                AutomataTransition *EtoS = new AutomataTransition;
                EtoS->from = left->finalStates[i];
                EtoS->to = left->start;
                EtoS->input = L"ε";
                left->finalStates[i]->transitions.push_back(EtoS);
                automata->transitions.push_back(EtoS);
                toE->from = left->finalStates[i];
                toE->to = end;
                toE->input = L"ε";
                left->finalStates[i]->transitions.push_back(toE);
                automata->transitions.push_back(toE);
            }

            if (wcscmp(node->value->token, L"*") == 0)
            {
                AutomataTransition *StoE = new AutomataTransition;
                StoE->from = start;
                StoE->to = end;
                StoE->input = L"ε";
                start->transitions.push_back(StoE);
                automata->transitions.push_back(StoE);
            }

            addEpsilon(alphabet);
            automata->alphabet = alphabet;
            automata->states.push_back(start);
            automata->states.push_back(end);
            automata->finalStates.push_back(end);
        }
        else if (wcscmp(node->value->token, L"?") == 0)
        {
            Automata *left = automataList->back();
            automata = left;
            automata->start->isAcceptable = false;

            for (int i = 0; i < automata->finalStates.size(); i++)
            {
                automata->finalStates[i]->isAcceptable = true;
                AutomataTransition *toE = new AutomataTransition;
                toE->from = automata->start;
                toE->to = automata->finalStates[i];
                toE->input = L"ε";
                automata->start->transitions.push_back(toE);
                automata->transitions.push_back(toE);
            }

            addEpsilon(alphabet);
            automata->alphabet = alphabet;
        }
    }

    return automata;
}

Automata *thompson(TreeNode *node, wstring &alphabet)
{
    if (node == nullptr)
    {
        return nullptr;
    }

    vector<Automata *> *automataList = new vector<Automata *>;

    Automata *left = thompson(node->left, alphabet);
    Automata *right = thompson(node->right, alphabet);

    if (left != nullptr)
    {
        automataList->push_back(left);
    }

    if (right != nullptr)
    {
        automataList->push_back(right);
    }

    Automata *current = buildThompsonSnippet(node, alphabet, automataList);

    return current;
}

bool simulateAutomata(Automata *automata, wstring &input);
wstring simulateNFA(Automata *automata, wstring &input);

#include <set>
#include <omp.h>

bool simulateAutomata(Automata *automata, wstring &input)
{
    AutomataState *currentState = automata->start;
    wstring processedInput; // Almacena la entrada procesada hasta ahora

    for (wchar_t symbol : input)
    {
        AutomataTransition *transition = nullptr;
        for (AutomataTransition *t : automata->transitions)
        {
            /* wcout << L"Transition from " << t->from->name << L" to " << t->to->name << L" with input " << *t->input << endl; */
            if (wstring(t->from->name) == wstring(currentState->name) /*  && t->input == &symbol */)
            {
                /* wcout << L"Transition from " << t->from->name << L" to " << t->to->name << L" with input " << *t->input << endl; */
                wstring s = wstring(1, *t->input);
                wstring d = wstring(1, symbol);

                /* wcout << (s == d) << endl; */

                if (s == d)
                {
                    /* wcout << L"Founded " << endl; */
                    transition = t;
                    break;
                }
            }
        }
        processedInput += symbol; // Agrega el símbolo procesado a la entrada procesada
        if (transition == nullptr)
        {
            if (!processedInput.empty())
            {
                wstring allButLast = processedInput.substr(0, processedInput.size() - 1);
                wchar_t lastChar = processedInput.back();

                /* wcerr << L"\033[1;31m"
                      << L"ERROR: \033[0m" << L"Input refused " << allButLast << "\033[1;31m" << lastChar
                      << "<-"
                      << L"\033[0m" << endl; */
            }
            else
            {
                /* wcerr << L"\033[1;31m"
                      << L"ERROR: \033[0m" << L"Input refused <-"
                      << L"\033[0m" << endl; */
            }
            return false;
        }
        currentState = transition->to;
    }

    if (currentState->isAcceptable)
    {
        /* wcout << L"Final state: " << currentState->name << endl; */
        /* wcout << L"\033[1;32mInput: " << processedInput << L" accepted\033[0m" << endl; */
        return true;
    }
    else
    {
        /* wcout << L"Final state: " << currentState->name << endl; */
        if (!processedInput.empty())
        {
            wstring allButLast = processedInput.substr(0, processedInput.size() - 1);
            wchar_t lastChar = processedInput.back();

            /*  wcerr << L"\033[1;31m"
                   << L"ERROR: \033[0m" << L"Input refused " << allButLast << "\033[1;31m" << lastChar
                   << "<-"
                   << L"\033[0m" << endl; */
        }
        else
        {
            /* wcerr << L"\033[1;31m"
                  << L"ERROR: \033[0m" << L"Input refused <-"
                  << L"\033[0m" << endl; */
        }
        return false;
    }
}

void printError(wstring &processedInput)
{
    if (!processedInput.empty())
    {
        wstring allButLast = processedInput.substr(0, processedInput.size() - 1);
        wchar_t lastChar = processedInput.back();

        wcerr << L"\033[1;31m"
              << L"ERROR: \033[0m" << L"Input refused " << allButLast << "\033[1;31m" << lastChar
              << "<-"
              << L"\033[0m" << endl;
    }
    else
    {
        wcerr << L"\033[1;31m"
              << L"ERROR: \033[0m" << L"Input refused <-"
              << L"\033[0m" << endl;
    }
}

wstring simulateNFA(Automata *automata, wstring &input)
{
    set<AutomataState *> currentStates;
    currentStates.insert(automata->start);
    wstring processedInput; // Almacena la entrada procesada hasta ahora

    // Procesa las transiciones ε iniciales
    set<AutomataState *> visitedStates;
    bool epsilonTransitionExists;
    do
    {
        epsilonTransitionExists = false;
        set<AutomataState *> newStates;

        for (int i = 0; i < automata->transitions.size(); ++i)
        {
            AutomataTransition *t = automata->transitions[i];
            if (currentStates.find(t->from) != currentStates.end() && *t->input == L'ε' && visitedStates.find(t->to) == visitedStates.end())
            {
                {
                    newStates.insert(t->to);
                    epsilonTransitionExists = true;
                    visitedStates.insert(t->to);
                }
            }
        }
        currentStates.insert(newStates.begin(), newStates.end());
    } while (epsilonTransitionExists);

    for (int i = 0; i < input.size(); ++i)
    {
        wchar_t symbol = input[i];
        set<AutomataState *> newStates;

        for (int j = 0; j < automata->transitions.size(); ++j)
        {
            AutomataTransition *t = automata->transitions[j];
            if (currentStates.find(t->from) != currentStates.end() && *t->input == symbol)
            {
                {
                    newStates.insert(t->to);
                }
            }
        }
        currentStates = newStates;

        // Procesa las transiciones ε
        visitedStates.clear();
        do
        {
            epsilonTransitionExists = false;
            set<AutomataState *> newStates;

            for (int k = 0; k < automata->transitions.size(); ++k)
            {
                AutomataTransition *t = automata->transitions[k];
                if (currentStates.find(t->from) != currentStates.end() && *t->input == L'ε' && visitedStates.find(t->to) == visitedStates.end())
                {
                    {
                        newStates.insert(t->to);
                        epsilonTransitionExists = true;
                        visitedStates.insert(t->to);
                    }
                }
            }
            currentStates.insert(newStates.begin(), newStates.end());
        } while (epsilonTransitionExists);

        processedInput += symbol; // Agrega el símbolo procesado a la entrada procesada
        if (currentStates.empty())
        {
            /* printError(processedInput); */
            return L"";
        }
    }

// Verifica si alguno de los estados actuales es aceptable
    for (AutomataState *state : currentStates)
    {
        if (state->isAcceptable)
        {
            /* wcout << L"\033[1;32mInput: " << processedInput << L" accepted\033[0m" << endl; */
            return state->returnType;
        }
    }

    /* printError(processedInput); */
    return L"";
}

Automata *joinAutomatas(vector<Automata *> automatas)
{
    Automata *newAutomata = new Automata();
    wstring alphabet = L"";
    newAutomata->alphabet = alphabet;
    AutomataState *newStart = new AutomataState();
    newStart->name = L"START";
    newStart->isAcceptable = false;
    newAutomata->start = newStart;

    // Ahora el nuevo estado inicial apunta a los estados iniciales de los autómatas originales con transición epsilon
    for (Automata *automata : automatas)
    {
        AutomataTransition *newTransition = new AutomataTransition();
        newTransition->from = newStart;
        newTransition->to = automata->start;
        newTransition->input = L"ε";
        newAutomata->transitions.push_back(newTransition);
    }

    // Incluir todos los caracteres de los alfabetos de cada autómata al nuevo alfabeto, sin repetir caracteres
    for (Automata *automata : automatas)
    {
        for (wchar_t symbol : automata->alphabet)
        {
            if (alphabet.find(symbol) == wstring::npos)
            {
                alphabet += symbol;
            }
        }
    }

    // Copiar todas las transiciones de los autómatas originales al nuevo autómata
    for (Automata *automata : automatas)
    {
        for (AutomataTransition *transition : automata->transitions)
        {
            AutomataTransition *newTransition = new AutomataTransition();
            newTransition->from = transition->from;
            newTransition->to = transition->to;
            newTransition->input = transition->input;
            newAutomata->transitions.push_back(newTransition);
        }
    }

    // Copiar todos los estados finales de los autómatas originales al nuevo autómata
    for (Automata *automata : automatas)
    {
        for (AutomataState *state : automata->finalStates)
        {
            wcout << L"Final state: " << state->name << endl;
            state->isAcceptable = true;
            newAutomata->finalStates.push_back(state);
        }
    }

    return newAutomata;
}

struct Symbol
{
    wchar_t *type;
    wchar_t *value;
    int numberLine;
    int numberColumn;
    Symbol *pointer;
};

Stack<shuntingToken> postfix;
Stack<shuntingToken> postfixAugmented;
Automata *automatas;

struct Tables
{
    vector<Symbol *> SymbolTable;
    vector<Symbol *> ErrorTable;
};

vector<Automata *> addRegex(vector<Automata *> automatas, wstring regex, wstring returnType)
{
    postfix = shuntingYard(regex.c_str());
    TreeNode *tree = constructSyntaxTree(&postfix);
    wstring alphabet = getAlphabet(&postfix);
    Automata *mcythompson = thompson(tree, alphabet);
    mcythompson = addReturnType(mcythompson, returnType);
    automatas.push_back(mcythompson);
    return automatas;
}

#include <map>
#include <string>


Automata *configYalexAutomata()
{
    vector<Automata *> automatas;
    automatas = addRegex(automatas, L":=", L"ASSIGNOP");
    automatas = addRegex(automatas, L"/", L"DIV");
    automatas = addRegex(automatas, L"=", L"EQ");
    automatas = addRegex(automatas, L"(' ')+[A-Za-z]([A-Za-z]|( )*|[0-9])*", L"ID");
    automatas = addRegex(automatas, L"\(", L"LPAREN");
    automatas = addRegex(automatas, L"-", L"MINUS");
    automatas = addRegex(automatas, L"[0-9]+(\.[0-9]+)?(E[\+-]?[0-9]+)?", L"NUMBER");
    automatas = addRegex(automatas, L"\+", L"PLUS");
    automatas = addRegex(automatas, L"'\)'", L"RPAREN");
    automatas = addRegex(automatas, L";", L"SEMICOLON");
    automatas = addRegex(automatas, L"\*", L"TIMES");
 return joinAutomatas(automatas);
}

#include <iostream>

void generateGraph(Automata *automata, wstring filename);

#include <fstream>
#include <string>
#include <locale>
#include <codecvt>
#include <algorithm>

using namespace std;

string escapeSpecialChars(const string &input)
{
    string output = input;
    size_t pos = 0;
    while ((pos = output.find('\\', pos)) != string::npos)
    {
        output.replace(pos, 1, "\\\\");
        pos += 2;
    }
    pos = 0;
    while ((pos = output.find('"', pos)) != string::npos)
    {
        output.replace(pos, 1, "\\\"");
        pos += 2;
    }
    pos = 0;
    while ((pos = output.find('(', pos)) != string::npos)
    {
        output.replace(pos, 1, "\\(");
        pos += 2;
    }
    pos = 0;
    while ((pos = output.find(')', pos)) != string::npos)
    {
        output.replace(pos, 1, "\\)");
        pos += 2;
    }
    return output;
}

void generateGraph(Automata *automata, wstring filename)
{
    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
    string strFilename = converter.to_bytes(filename);
    ofstream out(strFilename + ".dot");

    string dotScript = "digraph finite_state_machine {\n";
    dotScript += "rankdir=LR;\n";
    dotScript += "size=\"800,800\"\n";
    dotScript += "node [shape = plaintext]; qi [label=\"\"];\n";
    if (automata->start->isAcceptable)
    {
        dotScript += "node [shape = doublecircle];\n";
    }
    else
    {
        dotScript += "node [shape = circle];\n";
    }
    wstring startStateNameW(automata->start->name);
    string startStateName = converter.to_bytes(startStateNameW);
    dotScript += "qi -> " + startStateName + ";\n";

    for (const auto &state : automata->states)
    {
        wstring stateNameW(state->name);
        string stateName = converter.to_bytes(stateNameW);

        if (state->isAcceptable)
        {
            wstring returnTypeW(state->returnType);
            string returnType = converter.to_bytes(returnTypeW);
            dotScript += "node [shape = doublecircle, label=\"" + stateName + "\\nreturns: " + returnType + "\"];\n";
        }
        else
        {
            dotScript += "node [shape = circle]; " + stateName + ";\n";
        }
    }

    for (const auto &transition : automata->transitions)
    {
        wstring fromStateNameW(transition->from->name);
        string fromStateName = converter.to_bytes(fromStateNameW);

        wstring toStateNameW(transition->to->name);
        string toStateName = converter.to_bytes(toStateNameW);

        wstring inputW(transition->input);
        string input = converter.to_bytes(inputW);

        // Escape special characters
        input = escapeSpecialChars(input);

        dotScript += fromStateName + " -> " + toStateName + " [ label = \"" + input + "\" ];\n";
    }

    dotScript += "}\n";

    out << dotScript;
    out.close();

    string svgFilename = strFilename.substr(0, strFilename.find_last_of(".")) + ".svg";
    string command = "dot -Tsvg " + strFilename + ".dot" + " -o " + svgFilename;
    system(command.c_str());
}

Tables analyzeLexical(Automata *automata, wstring strText)
{
    vector<Symbol *> SymbolTable = vector<Symbol *>();
    vector<Symbol *> ErrorTable = vector<Symbol *>();
    Tables tables = Tables();

    int fileSize = strText.length();
    int processedSize = 0;

    wcout << strText << endl;

    wstring::iterator begin = strText.begin();
    wstring::iterator end = strText.end();
    wstring::iterator current = strText.begin();
    wstring::iterator latestAccepted = strText.begin();
    wstring::iterator latestRejected = strText.begin();
    int numberLine = 1;
    int numberCol = 1;

    wstring latestAcceptedSymbol = L"";

    // Define los delimitadores
    wstring delimiters = L"\t\n";

    vector<wstring> tokens;
    size_t startString = 0;
    size_t endString = strText.find_first_of(delimiters);

    while (endString != wstring::npos)
    {
        // Añade el token, incluyendo el delimitador
        tokens.push_back(strText.substr(startString, endString - startString + 1));

        // Encuentra el inicio del siguiente token
        startString = endString + 1;
        endString = strText.find_first_of(delimiters, startString);
    }

    // Añade el último token si no termina con un delimitador
    if (startString != strText.length())
    {
        tokens.push_back(strText.substr(startString));
    }

    for (const wstring &token : tokens)
    {
        /* wcout << token << L" " << token.length() << endl; */
        processedSize += token.length();
        wcout << L"Processed: " << processedSize << L" of " << fileSize << endl;
        strText = token;
        while (strText.length() > 0)
        {
            while (current != strText.end())
            {
                wstring substring = strText.substr(0, distance(begin, current + 1));
                wstring result = simulateNFA(automata, substring);

                if (result != L"")
                {
                    if (*current == L'\n')
                    {
                        numberLine++;
                        numberCol = 1;
                    }
                    else
                    {
                        numberCol = substring.length() - substring.rfind(L'\n');
                    }

                    latestAccepted = current;
                    latestAcceptedSymbol = result;
                    latestRejected = begin;
                }
                else
                {
                    latestRejected = current;
                }
                current++;
            }

            if (latestAcceptedSymbol != L"" || latestAccepted != begin)
            {
                wstring acceptedSubstring = strText.substr(0, distance(begin, latestAccepted + 1));
                Symbol *symbol = new Symbol();
                symbol->value = wcsdup(acceptedSubstring.c_str());
                symbol->type = wcsdup(latestAcceptedSymbol.c_str());
                symbol->numberLine = numberLine;
                symbol->numberColumn = numberCol;
                SymbolTable.push_back(symbol);
                strText.erase(0, distance(begin, latestAccepted + 1));
                begin = strText.begin();
                current = strText.begin();
                latestAccepted = strText.begin();
                latestRejected = strText.begin();
                end = strText.end();
                latestAcceptedSymbol = L"";
            }
            else
            {
                latestAcceptedSymbol = L"";
                wstring rejectedSubstring = strText.substr(0, distance(begin, latestRejected + 1));

                wstring::iterator beginRejected = begin;
                wstring::iterator endRejected = latestRejected;
                wstring::iterator evalCursor = beginRejected;
                wstring::iterator delimiter;

                wstring refusedToken = L"";

                while (evalCursor != endRejected)
                {
                    int bias = distance(evalCursor, endRejected) + 1;
                    int startSubs = distance(beginRejected, evalCursor);

                    for (int j = 1; j <= bias; j++)
                    {
                        wstring substringEval = strText.substr(startSubs, j);
                        wstring resultSim = simulateNFA(automata, substringEval);

                        if (resultSim != L"")
                        {
                            refusedToken = strText.substr(0, startSubs);
                            delimiter = evalCursor;
                            break;
                        }
                    }
                    if (delimiter == evalCursor)
                    {
                        break;
                    }
                    evalCursor++;
                }

                if (evalCursor == endRejected)
                {
                    wstring substringEval = strText.substr(distance(begin, evalCursor), 1);
                    wstring resultSim = simulateNFA(automata, substringEval);

                    if (resultSim != L"")
                    {
                        refusedToken = strText.substr(0, distance(beginRejected, evalCursor));
                        delimiter = evalCursor;
                    }
                }

                Symbol *symbol = new Symbol();
                if (refusedToken == L"")
                {
                    symbol->value = wcsdup(rejectedSubstring.c_str());
                    symbol->type = L"Error";
                    symbol->numberLine = numberLine;
                    symbol->numberColumn = numberCol;
                    ErrorTable.push_back(symbol);
                    strText.erase(0, distance(begin, latestRejected + 1));
                    begin = strText.begin();
                    current = strText.begin();
                    latestAccepted = strText.begin();
                    latestRejected = strText.begin();
                    end = strText.end();
                }
                else
                {
                    symbol->value = wcsdup(refusedToken.c_str());
                    symbol->type = L"Error";
                    symbol->numberLine = numberLine;
                    symbol->numberColumn = numberCol;
                    ErrorTable.push_back(symbol);
                    strText.erase(0, distance(begin, delimiter));
                    begin = strText.begin();
                    current = strText.begin();
                    latestAccepted = strText.begin();
                    latestRejected = strText.begin();
                    end = strText.end();
                }
            }

            if (current == strText.end())
            {
                break;
            }
        }
    }

    tables.SymbolTable = SymbolTable;
    tables.ErrorTable = ErrorTable;

    return tables;
}

#include <fstream>
#include <sstream>
#include <locale>
#include <codecvt>

wstring cleanText(wstring text)
{
    size_t pos = 0;
    while ((pos = text.find(L"(*", pos)) != wstring::npos)
    {
        size_t endPos = text.find(L"*)", pos);
        if (endPos != wstring::npos)
        {
            text.erase(pos, endPos - pos + 2); // +2 to remove "*)"
        }
        else
        {
            break; // No closing "*)", so we break the loop
        }
    }
    return text;
}

int main(int argc, char *argv[])
{
    vector<Symbol *> SymbolTable = vector<Symbol *>();
    vector<Symbol *> ErrorTable = vector<Symbol *>();

    if (argc < 2)
    {
        std::wcerr << L"Por favor, proporciona la ruta del archivo como argumento." << std::endl;
        return 1;
    }

    std::locale old_locale;
    std::locale utf8_locale(old_locale, new std::codecvt_utf8<wchar_t>);
    std::wifstream wif(argv[1]);
    if (!wif)
    {
        std::wcerr << L"No se pudo abrir el archivo." << std::endl;
        return 1;
    }
    wif.imbue(utf8_locale);

    std::wstringstream wss;
    wss << wif.rdbuf();
    std::wstring contents = wss.str();

    automatas = configYalexAutomata();
    printAutomata(automatas);

    wstring cleanedText = cleanText(contents);

    Tables tables = analyzeLexical(automatas, cleanedText);

    SymbolTable = tables.SymbolTable;
    ErrorTable = tables.ErrorTable;

    wcout << endl;
    wcout << "Symbol Table" << endl;

    // Elimina los tokens de tipo "Line Break" de la tabla de símbolos
    /* SymbolTable.erase(std::remove_if(SymbolTable.begin(), SymbolTable.end(),
                                     [](Symbol *symbol)
                                     { return wcscmp(symbol->type, L"Line Break") == 0; }),
                      SymbolTable.end()); */

    for (Symbol *symbol : SymbolTable)
    {
        wcout << L"Token: " << symbol->value << L" Type: " << symbol->type << L" Line: " << symbol->numberLine << L" Column: " << symbol->numberColumn << endl;
    }
    return 0;
}

