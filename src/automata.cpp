#include "automata.h"

wstring getAlphabet(Stack<shuntingToken> *postfix)
#include <cwchar>

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