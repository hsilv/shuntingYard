#ifndef DIRECT_H
#define DIRECT_H

#include "automata.h"
#include "tree.h"
#include "shunting.h"

/* Automata *buildDirectSnippet(TreeNode *node, wstring &alphabet, vector<Automata *> *automataList);
Automata *directConstruction(TreeNode *node, wstring &alphabet); */
Automata *directConstruction(TreeNode *node, wstring &alphabet);
TreeNode *tagLeaves(TreeNode *node);
bool anullableFunction(TreeNode *node);
set<wstring> *firstPosFunction(TreeNode *node);
set<wstring> *lastPosFunction(TreeNode *node);

#endif