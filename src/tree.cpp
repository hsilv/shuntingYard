#include "tree.h"
#include "shunting.h"

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
            }
            else
            {
                node->right = treeStack.pop();
                node->left = treeStack.pop();
            }
            treeStack.push(node);
        }
    }

    return treeStack.pop();
}

void print2DUtil(TreeNode *root, int space)
{
    const int COUNT = 10;
    if (root == NULL)
        return;

    space += COUNT;

    print2DUtil(root->right, space);

    wcout << endl;
    for (int i = COUNT; i < space; i++)
        wcout << L" ";

    if (wcscmp(root->value->token, L"") != 0)
    {
        wcout << L"\033[1;31m" << root->tag << L" \033[0m";
    }
    wcout << root->value->token << L"\n";

    print2DUtil(root->left, space);
}

void print2D(TreeNode *root)
{
    print2DUtil(root, 0);
}

TreeNode *parseTree(TreeNode *root)
{
    if (root == nullptr)
    {
        return nullptr;
    }

    shuntingToken *newToken = new shuntingToken;
    *newToken = *root->value;
    TreeNode *newRoot = createNode(newToken);
    newRoot->left = parseTree(root->left);
    newRoot->right = parseTree(root->right);

    if (wcscmp(newRoot->value->token, L"?") == 0)
    {
        newRoot->value->token = wcsdup(L"|");
        newRoot->value->type = shuntingToken::BINARY_OPERATOR;
        newRoot->value->precedence = 1;
        shuntingToken *epsilonToken = new shuntingToken;
        epsilonToken->token = wcsdup(L"ε");
        epsilonToken->precedence = 0;
        epsilonToken->type = shuntingToken::OPERAND;
        newRoot->right = createNode(epsilonToken);
    }
    else if (wcscmp(newRoot->value->token, L"+") == 0)
    {
        newRoot->value->token = wcsdup(L".");
        newRoot->value->type = shuntingToken::BINARY_OPERATOR;
        newRoot->value->precedence = 2;
        shuntingToken *starToken = new shuntingToken;
        starToken->token = wcsdup(L"*");
        starToken->precedence = 3;
        starToken->type = shuntingToken::UNARY_OPERATOR;
        TreeNode *starNode = createNode(starToken);
        starNode->left = parseTree(newRoot->left); // Create a copy of newRoot->left
        newRoot->right = starNode;
    }

    return newRoot;
}