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

    if (automata->transitions.size() < transitions->size())
    {
        automata->transitions = *transitions;
        automata->states.push_back(deadState);
    }
}