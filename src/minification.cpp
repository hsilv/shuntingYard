#include "minification.h"
#include <vector>

int minifiedStatesCounter = 0;

Automata *minifyAutomata(Automata *automata)
{

    /* Automata *minified = new Automata();
    vector<AutomataState *> *states = new vector<AutomataState *>();
    minified->states = *states;
    vector<AutomataTransition *> *transitions = new vector<AutomataTransition *>();
    minified->transitions = *transitions;
    AutomataState *startState = new AutomataState();
    minified->start = startState;
    vector<AutomataState *> *finalStates = new vector<AutomataState *>();
    minified->finalStates = *finalStates;
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

    bool partitionAdded;
    do
    {
        partitionAdded = false;
        Partition *base = nullptr;
        for (auto &partition : partitions)
        {
            for (auto &state : *partition->states)
            {
                if (wstring(state->name) == wstring(automata->start->name))
                {
                    base = partition;
                    break;
                }
            }
            if (base != nullptr)
            {
                break;
            }
        }
        vector<AutomataState *> toErase;

        for (const auto &symbol : alphabet)
        {
            map<wstring, set<AutomataState *>> transitions;
            for (const auto &state : *base->states)
            {
                for (const auto &transition : state->transitions)
                {
                    if (wstring(transition->input) == wstring(1, symbol))
                    {
                        wstring toStateName = wstring(transition->to->name);
                        if (transitions.find(toStateName) == transitions.end())
                        {
                            transitions[toStateName] = set<AutomataState *>();
                        }
                        transitions[toStateName].insert(state);
                    }
                }
            }

            for (const auto &pair : transitions)
            {
                if (pair.second.size() < base->states->size())
                {
                    Partition *newPartition = new Partition();
                    newPartition->states = new set<AutomataState *>(pair.second);
                    partitions.push_back(newPartition);
                    for (const auto &state : pair.second)
                    {
                        base->states->erase(state);
                    }
                    partitionAdded = true;
                }
            }
        }
    } while (partitionAdded);

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
    minified->alphabet = automata->alphabet; */

    return automata;
}