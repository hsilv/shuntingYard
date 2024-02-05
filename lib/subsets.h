#ifndef SUBSETS_H
#define SUBSETS_H

#include "stack.h"
#include "automata.h"
#include <vector>

using namespace std;

struct StateSubset
{
    vector<AutomataState *> *states;
    AutomataState *state;
    bool marked;
    vector<AutomataTransition *> *transitions;
};

vector<AutomataState *> *epsilonClosure(AutomataState *state);
vector<AutomataState *> *move(vector<AutomataState *> *states, wchar_t *input);
Automata *subsetConstruction(Automata *automata);
void processState(StateSubset *subset, vector<StateSubset *> *subsets, wstring alphabet, vector<AutomataState *> finalStates);

#endif