#ifndef GRAPHVIZ_H
#define GRAPHVIZ_H

#include <iostream>
#include "automata.h"
#include "yapar.h"

void generateGraph(Automata *automata, wstring filename);
void generateLR0Graph(LR0Automata *automata, wstring filename);

#endif