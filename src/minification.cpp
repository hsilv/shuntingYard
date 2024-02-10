#include "minification.h"
#include <vector>

int minifiedStatesCounter = 0;

Automata *minifyAutomata(Automata *automata)
{

    Automata *minified = new Automata();
    vector<AutomataState *> *states = new vector<AutomataState *>();
    minified->states = *states;
    vector<AutomataTransition *> *transitions = new vector<AutomataTransition *>();
    minified->transitions = *transitions;
    AutomataState *startState = new AutomataState();
    minified->start = startState;
    vector<AutomataState *> *finalStates = new vector<AutomataState *>();
    minified->finalStates = *finalStates;
    // Crear particiones iniciales
    minifiedStatesCounter = 0;
    vector<Partition *> partitions;
    wstring alphabet = automata->alphabet;

    Partition *SF = new Partition();
    Partition *SI = new Partition();
    set<AutomataState *> *SFStates = new set<AutomataState *>();
    set<AutomataState *> *SIStates = new set<AutomataState *>();
    SI->states = SIStates;
    SF->states = SFStates;

    for (auto state : automata->states)
    {
        if (state->isAcceptable)
        {
            SF->states->insert(state);
        }
        else
        {
            SI->states->insert(state);
        }
    }

    partitions.push_back(SI);
    partitions.push_back(SF);

    wcout << L"SF: ";
    for (const auto &item : *SF->states)
    {
        wcout << item->name << L" ";
    }
    wcout << endl;

    wcout << L"SI: ";
    for (const auto &item : *partitions[0]->states)
    {
        wcout << item->name << L" ";
    }
    wcout << endl;

    bool partitionAdded;
    do
    {
        partitionAdded = false;
        Partition *base = partitions[0];
        vector<AutomataState *> toErase;
        for (const auto &symbol : alphabet)
        {
            set<AutomataState *> *toBeAdded = new set<AutomataState *>();
            for (const auto &state : *base->states)
            {
                /* wcout << L"Trying with: " << state->name << L" and symbol: " << symbol << endl; */
                for (const auto &transition : automata->transitions)
                {
                    if (transition->from == state && transition->input == wstring(1, symbol))
                    {
                        /* wcout << L"Transition found: " << transition->from->name << L" -> " << transition->to->name << L" with symbol: " << transition->input << endl; */
                        if (base->states->find(transition->to) != base->states->end())
                        {
                            /* wcout << L"Transition target is in the base state set." << endl; */
                        }
                        else
                        {
                            /* wcout << L"Transition target is not in the base state set." << endl; */
                            toBeAdded->insert(transition->from);
                        }
                    }
                }
            }
            // Acá
            bool found = false;
            for (const auto &partition : partitions)
            {
                if (*partition->states == *toBeAdded)
                {
                    found = true;
                    break;
                }
            }

            if (!found && !toBeAdded->empty())
            {
                Partition *newPartition = new Partition();
                newPartition->states = toBeAdded;
                partitions.push_back(newPartition);
                partitionAdded = true;
                for (const auto &state : *toBeAdded)
                {
                    base->states->erase(state);
                }
            }
            else
            {
                delete toBeAdded;
            }
        }
    } while (partitionAdded);

    wcout << endl;

    for (auto partition : partitions)
    {
        partition->name = L"m" + to_wstring(minifiedStatesCounter++);
        AutomataState *stateReference = new AutomataState();
        vector<AutomataTransition *> *stateTransitions = new vector<AutomataTransition *>();
        stateReference->name = wcsdup(partition->name.c_str());
        stateReference->transitions = *stateTransitions;
        partition->stateReference = stateReference;
        wstring startStateName = automata->start->name; // Obtén el nombre del estado inicial

        auto it = std::find_if(partition->states->begin(), partition->states->end(),
                               [&startStateName](const auto &state)
                               { return state->name == startStateName; });

        if (it != partition->states->end())
        {
            partition->isInitial = true;
        }
        else
        {
            partition->isInitial = false;
        }
        partition->isFinal = false;
        stateReference->isAcceptable = false;
        for (const auto &finalState : automata->finalStates)
        {
            if (partition->states->find(finalState) != partition->states->end())
            {
                partition->isFinal = true;
                stateReference->isAcceptable = true;
                break;
            }
        }
        states->push_back(stateReference);
    }

    AutomataState *trapState = new AutomataState();
    trapState->name = L"trap";

    states->push_back(trapState);

    for (auto partition : partitions)
    {
        for (const auto &symbol : automata->alphabet)
        {
            bool transitionAdded = false;
            for (const auto &transition : automata->transitions)
            {
                if (!transitionAdded && partition->states->find(transition->from) != partition->states->end() && transition->input == wstring(1, symbol))
                {
                    for (const auto &targetPartition : partitions)
                    {
                        if (targetPartition->states->find(transition->to) != targetPartition->states->end())
                        {

                            bool transitionExists = false;
                            for (const auto &existingTransition : automata->transitions)
                            {
                                if (existingTransition->from == partition->stateReference && existingTransition->to == targetPartition->stateReference && existingTransition->input == wstring(1, symbol))
                                {
                                    transitionExists = true;
                                    break;
                                }
                            }

                            if (!transitionExists)
                            {
                                AutomataTransition *newTransition = new AutomataTransition();
                                newTransition->from = partition->stateReference;
                                newTransition->to = targetPartition->stateReference;
                                newTransition->input = wcsdup(wstring(1, symbol).c_str());

                                transitions->push_back(newTransition);
                                partition->stateReference->transitions.push_back(newTransition);
                                transitionAdded = true;
                            }
                            break;
                        }
                    }
                }
            }
            if (!transitionAdded)
            {
                AutomataTransition *trapTransition = new AutomataTransition();
                trapTransition->from = partition->stateReference;
                trapTransition->to = trapState;
                trapTransition->input = wcsdup(wstring(1, symbol).c_str());

                transitions->push_back(trapTransition);
                partition->stateReference->transitions.push_back(trapTransition);
            }
        }
    }
    for (auto partition : partitions)
    {
        wcout << L"Partition: " << partition->name << L" ";
        for (const auto &item : *partition->states)
        {
            wcout << item->name << L" ";
        }
        wcout << endl;
    }

    if (trapState != nullptr)
    {

        bool isTrapStateUsed = false;
        for (auto transition : *transitions)
        {
            if (transition->from == trapState || transition->to == trapState)
            {
                isTrapStateUsed = true;
                break;
            }
        }

        if (!isTrapStateUsed)
        {
            states->erase(std::remove(states->begin(), states->end(), trapState), states->end());
        }
    }

    for (auto state : *states)
    {
        wcout << state->name << endl;
        wcout << state->isAcceptable << endl;

        for (auto transition : state->transitions)
        {
            wcout << L"  " << transition->input << L" -> " << transition->to->name << endl;
        }
    }

    for (auto partition : partitions)
    {
        if (partition->isInitial)
        {
            minified->start = partition->stateReference;
        }
        if (partition->isFinal)
        {
            minified->finalStates.push_back(partition->stateReference);
        }
    }

    minified->states = *states;
    minified->transitions = *transitions;
    minified->alphabet = automata->alphabet;

    return minified;
}