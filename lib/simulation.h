#ifndef SIMULATION_H
#define SIMULATION_H

#include "automata.h"

bool simulateAutomata(Automata *automata, wstring &input);
bool simulateNFA(Automata *automata, wstring &input);

#endif