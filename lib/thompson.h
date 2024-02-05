#ifndef THOMPSON_H
#define THOMPSON_H

#include <iostream>
#include "automata.h"
#include "tree.h"

Automata *buildThompsonSnippet(TreeNode *node, wstring &alphabet, vector<Automata *> *automataList);
Automata *thompson(TreeNode *node, wstring &alphabet);

#endif