#ifndef TREE_H
#define TREE_H

#include <iostream>
#include "shunting.h"
#include <set>
using namespace std;

struct TreeNode;

struct TreeNode
{
    shuntingToken *value;
    TreeNode *left;
    TreeNode *right;
    wstring tag = L"";
    bool anulable;
    set<wstring> *firstPos;
    set<wstring> *lastPos;
    set<wstring> *nextPos;
};

TreeNode *createNode(shuntingToken *value);
void deleteNode(TreeNode *node);
TreeNode *constructSyntaxTree(Stack<shuntingToken> *postfix);
void print2DUtil(TreeNode *root, int space);
void print2D(TreeNode *root);
TreeNode *parseTree(TreeNode *root);

#endif