#ifndef MINIFICATION_H
#define MINIFICATION_H

#include "automata.h"
#include <set>

struct Partition
{
    set<AutomataState *> *states;
    wstring symbol;
    wstring name;
    bool isInitial;
    bool isFinal;
    AutomataState *stateReference;
};

Automata *minifyAutomata(Automata *automata);

#endif