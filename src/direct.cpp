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
    else
    {
        node->anulable = false;
    }

    return node->anulable;
}

set<wstring> *firstPosFunction(TreeNode *node)
{
    set<wstring> *firstPos = new set<wstring>;
    if (wcscmp(node->value->token, L"|") == 0 && node->right != nullptr && node->left != nullptr)
    {
        set<wstring> *leftFirstPos = firstPosFunction(node->left);
        set<wstring> *rightFirstPos = firstPosFunction(node->right);
        firstPos->insert(leftFirstPos->begin(), leftFirstPos->end());
        firstPos->insert(rightFirstPos->begin(), rightFirstPos->end());
        return firstPos;
    }
    else if (wcscmp(node->value->token, L".") == 0 && node->right != nullptr && node->left != nullptr)
    {
        if (anullableFunction(node->left))
        {
            set<wstring> *leftFirstPos = firstPosFunction(node->left);
            set<wstring> *rightFirstPos = firstPosFunction(node->right);
            firstPos->insert(leftFirstPos->begin(), leftFirstPos->end());
            firstPos->insert(rightFirstPos->begin(), rightFirstPos->end());
            return firstPos;
        }
        else
        {
            set<wstring> *leftFirstPos = firstPosFunction(node->left);
            return leftFirstPos;
        }
    }
    else if (wcscmp(node->value->token, L"*") == 0)
    {
        set<wstring> *leftFirstPos = firstPosFunction(node->left);
        return leftFirstPos;
    }
    else if (wcscmp(node->value->token, L"+") == 0 && node->left != nullptr)
    {
        set<wstring> *leftFirstPos = firstPosFunction(node->left);
        return leftFirstPos;
    }
    else if (wcscmp(node->value->token, L"?") == 0)
    {
        set<wstring> *leftFirstPos = firstPosFunction(node->left);
        return leftFirstPos;
    }
    else
    {
        firstPos->insert(node->tag);
    }
    return firstPos;
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
    set<wstring> *firstPos = firstPosFunction(node);

    print2DUtil(node, 0);

    return automata;
}