#include "tree.h"

TreeNode *createNode(shuntingToken *value)
{
    TreeNode *node = new TreeNode;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void deleteNode(TreeNode *node)
{
    if (node->left != NULL)
    {
        deleteNode(node->left);
    }
    if (node->right != NULL)
    {
        deleteNode(node->right);
    }
    delete node;
    return;
}

TreeNode *constructSyntaxTree(Stack<shuntingToken> *postfix)
{
    Stack<TreeNode *> treeStack;

    for (int i = 0; i < postfix->getSize(); i++)
    {
        shuntingToken token = postfix->getTop()[-i];
        shuntingToken *tokenCopy = new shuntingToken(token);

        if (token.type == shuntingToken::OPERAND)
        {
            TreeNode *node = createNode(tokenCopy);
            treeStack.push(node);
        }
        else
        {
            TreeNode *node = createNode(tokenCopy);
            if (token.type == shuntingToken::UNARY_OPERATOR)
            {
                node->left = treeStack.pop();
                wcout << "Root: " << node->value->token << " ";
                wcout << "Left: " << node->left->value->token << " " << endl;
            }
            else
            {
                node->right = treeStack.pop();
                node->left = treeStack.pop();
                wcout << "Root: " << node->value->token << " ";
                wcout << "Left: " << node->left->value->token << " ";
                wcout << "Right: " << node->right->value->token << endl;
            }
            treeStack.push(node);
        }
    }

    return treeStack.pop();
}