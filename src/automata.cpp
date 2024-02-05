#include "automata.h"
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
    }
}