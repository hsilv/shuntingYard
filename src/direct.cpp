#include "direct.h"

static int tagCounter = 1;
wstring generateTag()
{
    return to_wstring(tagCounter++);
}

bool anullableFunction(TreeNode *node)
{
    if (node->tag != L"")
    {
        node->anulable = false;
        if (node->right != nullptr)
        {
            anullableFunction(node->right);
        }
        if (node->left != nullptr)
        {
            anullableFunction(node->left);
        }
    }

    else if (wcscmp(node->value->token, L"|") == 0 && node->right != nullptr && node->left != nullptr)
    {
        const bool leftAnullable = anullableFunction(node->left);
        const bool rightAnullable = anullableFunction(node->right);
        node->anulable = leftAnullable || rightAnullable;
    }
    else if (wcscmp(node->value->token, L".") == 0 && node->right != nullptr && node->left != nullptr)
    {
        const bool leftAnullable = anullableFunction(node->left);
        const bool rightAnullable = anullableFunction(node->right);
        node->anulable = leftAnullable && rightAnullable;
    }
    else if (wcscmp(node->value->token, L"*") == 0)
    {
        node->anulable = true;
        if (node->right != nullptr)
        {
            anullableFunction(node->right);
        }
        if (node->left != nullptr)
        {
            anullableFunction(node->left);
        }
    }
    else if (wcscmp(node->value->token, L"+") == 0 && node->left != nullptr)
    {
        node->anulable = anullableFunction(node->left);
        if (node->right != nullptr)
        {
            anullableFunction(node->right);
        }
    }
    else if (wcscmp(node->value->token, L"?") == 0)
    {
        node->anulable = true;
        if (node->right != nullptr)
        {
            anullableFunction(node->right);
        }
        if (node->left != nullptr)
        {
            anullableFunction(node->left);
        }
    }

    return node->anulable;
}

TreeNode *tagLeaves(TreeNode *node)
{
    if (node == nullptr)
    {
        return nullptr;
    }

    if (node->left == nullptr && node->right == nullptr)
    {
        node->tag = generateTag();
    }
    else
    {
        tagLeaves(node->left);
        tagLeaves(node->right);
    }

    return node;
}

Automata *directConstruction(TreeNode *node, wstring &alphabet)
{
    Automata *automata = new Automata;

    node = tagLeaves(node);
    anullableFunction(node);

    print2DUtil(node, 0);

    return automata;
}