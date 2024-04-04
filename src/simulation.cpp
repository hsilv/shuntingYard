#include "simulation.h"
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

#pragma omp parallel for
        for (int i = 0; i < automata->transitions.size(); ++i)
        {
            AutomataTransition *t = automata->transitions[i];
            if (currentStates.find(t->from) != currentStates.end() && *t->input == L'ε' && visitedStates.find(t->to) == visitedStates.end())
            {
#pragma omp critical
                {
                    newStates.insert(t->to);
                    epsilonTransitionExists = true;
                    visitedStates.insert(t->to);
                }
            }
        }
        currentStates.insert(newStates.begin(), newStates.end());
    } while (epsilonTransitionExists);

#pragma omp parallel for
    for (int i = 0; i < input.size(); ++i)
    {
        wchar_t symbol = input[i];
        set<AutomataState *> newStates;

#pragma omp parallel for
        for (int j = 0; j < automata->transitions.size(); ++j)
        {
            AutomataTransition *t = automata->transitions[j];
            if (currentStates.find(t->from) != currentStates.end() && *t->input == symbol)
            {
#pragma omp critical
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

#pragma omp parallel for
            for (int k = 0; k < automata->transitions.size(); ++k)
            {
                AutomataTransition *t = automata->transitions[k];
                if (currentStates.find(t->from) != currentStates.end() && *t->input == L'ε' && visitedStates.find(t->to) == visitedStates.end())
                {
#pragma omp critical
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
#pragma omp parallel for
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