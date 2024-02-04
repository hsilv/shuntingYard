#ifndef TREE_H
#define TREE_H

#include <iostream>
#include "shunting.h"

using namespace std;

struct TreeNode;

struct TreeNode
{
    shuntingToken *value;
    TreeNode *left;
    TreeNode *right;
};

TreeNode *createNode(shuntingToken *value);
void deleteNode(TreeNode *node);
TreeNode *constructSyntaxTree(Stack<shuntingToken> *postfix);

#endif