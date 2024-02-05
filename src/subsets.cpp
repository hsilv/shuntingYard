#include "subsets.h"
#include <set>

using namespace std;

int subsetCounter = 0;

wstring getSubsetName()
{
    return L"a" + to_wstring(subsetCounter++);
}

vector<AutomataState *> *epsilonClosure(AutomataState *state, set<AutomataState *> &visited)
{
    vector<AutomataState *> *closure = new vector<AutomataState *>;
    closure->push_back(state);
    visited.insert(state);

    for (int i = 0; i < state->transitions.size(); i++)
    {
        if (wcscmp(state->transitions[i]->input, L"ε") == 0)
        {
            if (visited.find(state->transitions[i]->to) == visited.end())
            {
                vector<AutomataState *> *subClosure = epsilonClosure(state->transitions[i]->to, visited);
                closure->insert(closure->end(), subClosure->begin(), subClosure->end());
            }
        }
    }

    return closure;
}

vector<AutomataState *> *move(vector<AutomataState *> *states, wchar_t *input)
{
    vector<AutomataState *> *move = new vector<AutomataState *>;
    for (int i = 0; i < states->size(); i++)
    {
        for (int j = 0; j < states->at(i)->transitions.size(); j++)
        {

            if (wcscmp(states->at(i)->transitions[j]->input, input) == 0)
            {
                move->push_back(states->at(i)->transitions[j]->to);
            }
        }
    }
    return move;
}

void processState(StateSubset *subset, vector<StateSubset *> *subsets, wstring alphabet, vector<AutomataState *> finalStates)
{
    subset->marked = true;
    for (int j = 0; j < alphabet.size(); j++)
    {
        wchar_t character = alphabet[j];
        if (character != L'ε')
        {
            wchar_t characterString[2] = {character, '\0'};
            set<AutomataState *> moveVisited;
            vector<AutomataState *> *moveResult = move(subset->states, characterString);
            set<AutomataState *> totalClosureSet;

            totalClosureSet.insert(moveResult->begin(), moveResult->end());

            for (int k = 0; k < moveResult->size(); k++)
            {
                vector<AutomataState *> *closure = epsilonClosure(moveResult->at(k), moveVisited);
                totalClosureSet.insert(closure->begin(), closure->end());
            }

            vector<AutomataState *> totalClosure(totalClosureSet.begin(), totalClosureSet.end());

            StateSubset *newSubset = new StateSubset;
            newSubset->states = new vector<AutomataState *>(totalClosure);
            newSubset->state = new AutomataState;
            newSubset->state->name = wcsdup(getSubsetName().c_str());
            newSubset->state->isAcceptable = false;

            newSubset->marked = false;
            vector<AutomataTransition *> *newTransitions = new vector<AutomataTransition *>;
            newSubset->transitions = newTransitions;

            // Check if any of the states in the subset is a final state in the original automata
            for (AutomataState *state : *newSubset->states)
            {
                if (find(finalStates.begin(), finalStates.end(), state) != finalStates.end())
                {
                    newSubset->state->isAcceptable = true;
                    break;
                }
            }

            bool subsetExists = false;
            StateSubset *existingSubset = nullptr;
            for (int k = 0; k < subsets->size(); k++)
            {
                existingSubset = subsets->at(k);
                if (existingSubset->states->size() == newSubset->states->size())
                {
                    bool allStatesMatch = true;
                    for (int l = 0; l < existingSubset->states->size(); l++)
                    {
                        if (existingSubset->states->at(l) != newSubset->states->at(l))
                        {
                            allStatesMatch = false;
                            break;
                        }
                    }
                    if (allStatesMatch)
                    {
                        subsetExists = true;
                        break;
                    }
                }
            }

            AutomataTransition *transition = new AutomataTransition;
            transition->from = subset->state;
            transition->input = wcsdup(characterString);

            if (subsetExists)
            {
                transition->to = existingSubset->state;
                subset->transitions->push_back(transition);
            }
            else if (!subsetExists)
            {
                transition->to = newSubset->state;
                subset->transitions->push_back(transition);
                subsets->push_back(newSubset);
            }
        }
    }
}

Automata *subsetConstruction(Automata *automata)
{

    Automata *AFD = new Automata;
    set<AutomataState *> visited;
    vector<AutomataState *> *initialState = epsilonClosure(automata->start, visited);

    StateSubset *initialSubset = new StateSubset;
    initialSubset->states = initialState;
    initialSubset->state = new AutomataState;
    initialSubset->state->name = wcsdup(getSubsetName().c_str());
    initialSubset->state->isAcceptable = false;

    for (AutomataState *state : *initialState)
    {
        if (find(automata->finalStates.begin(), automata->finalStates.end(), state) != automata->finalStates.end())
        {
            initialSubset->state->isAcceptable = true;
            break;
        }
    }

    initialSubset->marked = true;
    vector<AutomataTransition *> *initialTransitions = new vector<AutomataTransition *>;
    initialSubset->transitions = initialTransitions;

    vector<StateSubset *> *subsets = new vector<StateSubset *>;

    subsets->push_back(initialSubset);

    wstring alphabet = automata->alphabet;

    processState(initialSubset, subsets, alphabet, automata->finalStates);

    for (int i = 0; i < subsets->size(); i++)
    {
        StateSubset *subset = subsets->at(i);
        if (!subset->marked)
        {
            processState(subset, subsets, alphabet, automata->finalStates);
        }
    }

    vector<AutomataTransition *> *AFDTransitions = new vector<AutomataTransition *>;
    vector<AutomataState *> *AFDStates = new vector<AutomataState *>;
    vector<AutomataState *> *AFDFinalStates = new vector<AutomataState *>;

    for (int i = 0; i < subsets->size(); i++)
    {
        AFDStates->push_back(subsets->at(i)->state);
        for (int j = 0; j < subsets->at(i)->transitions->size(); j++)
        {
            AFDTransitions->push_back(subsets->at(i)->transitions->at(j));
        }
        if (subsets->at(i)->state->isAcceptable)
        {
            AFDFinalStates->push_back(subsets->at(i)->state);
        }
    }

    AFD->start = initialSubset->state;
    AFD->alphabet = alphabet;
    AFD->states = *AFDStates;
    AFD->finalStates = *AFDFinalStates;
    AFD->transitions = *AFDTransitions;

    size_t pos = AFD->alphabet.find(L'ε');
    if (pos != std::wstring::npos)
    {
        AFD->alphabet.erase(pos, 1);
    }

    return AFD;
}