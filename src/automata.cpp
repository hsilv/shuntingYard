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