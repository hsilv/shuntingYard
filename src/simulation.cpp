#include "simulation.h"
#include <set>

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
            return false;
        }
        currentState = transition->to;
    }

    if (currentState->isAcceptable)
    {
        /* wcout << L"Final state: " << currentState->name << endl; */
        wcout << L"\033[1;32mInput: " << processedInput << L" accepted\033[0m" << endl;
        return true;
    }
    else
    {
        /* wcout << L"Final state: " << currentState->name << endl; */
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

bool simulateNFA(Automata *automata, wstring &input)
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
        for (AutomataState *state : currentStates)
        {
            for (AutomataTransition *t : automata->transitions)
            {
                if (wstring(t->from->name) == wstring(state->name) && *t->input == L'ε' && visitedStates.find(t->to) == visitedStates.end())
                {
                    newStates.insert(t->to);
                    epsilonTransitionExists = true;
                    visitedStates.insert(t->to);
                }
            }
        }
        currentStates.insert(newStates.begin(), newStates.end());
    } while (epsilonTransitionExists);

    for (wchar_t symbol : input)
    {
        set<AutomataState *> newStates;
        for (AutomataState *state : currentStates)
        {
            for (AutomataTransition *t : automata->transitions)
            {
                if (wstring(t->from->name) == wstring(state->name) && *t->input == symbol)
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
            for (AutomataState *state : currentStates)
            {
                for (AutomataTransition *t : automata->transitions)
                {
                    if (wstring(t->from->name) == wstring(state->name) && *t->input == L'ε' && visitedStates.find(t->to) == visitedStates.end())
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
            printError(processedInput);
            return false;
        }
    }

    // Verifica si alguno de los estados actuales es aceptable
    for (AutomataState *state : currentStates)
    {
        if (state->isAcceptable)
        {
            wcout << L"\033[1;32mInput: " << processedInput << L" accepted\033[0m" << endl;
            return true;
        }
    }

    printError(processedInput);
    return false;
}