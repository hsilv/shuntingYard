#ifndef AUTOMATA_H
#define AUTOMATA_H

#include <iostream>
#include <vector>

using namespace std;

struct AutomataTransition;
struct AutomataState;
struct Automata;

struct AutomataState
{
    wchar_t *name;
    vector<AutomataTransition *> transitions;
    bool isAcceptable;
};

struct AutomataTransition
{
    AutomataState *from;
    AutomataState *to;
    wchar_t *input;
};

struct Automata
{
    AutomataState *start;
    vector<AutomataState *> states;
    vector<AutomataState *> finalStates;
    wstring alphabet;
    vector<AutomataTransition *> transitions;
};

#endif