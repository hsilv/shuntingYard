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
    if (node == nullptr)
    {
        return nullptr;
    }
    set<wstring> *firstPos = new set<wstring>;
    bool isLeft = (node->left != nullptr);
    bool isRight = (node->right != nullptr);
    set<wstring> *leftFirstPos = firstPosFunction(node->left);
    set<wstring> *rightFirstPos = firstPosFunction(node->right);
    bool leftAnullable = (node->left != nullptr) ? node->left->anulable : false;

    if (node->tag != L"")
    {
        firstPos->insert(node->tag);
        node->firstPos = firstPos;
        return firstPos;
    }
    else if (wcscmp(node->value->token, L"|") == 0 && isLeft && isRight)
    {
        firstPos->insert(leftFirstPos->begin(), leftFirstPos->end());
        firstPos->insert(rightFirstPos->begin(), rightFirstPos->end());
        node->firstPos = firstPos;
        return firstPos;
    }
    else if (wcscmp(node->value->token, L".") == 0 && isLeft && isRight)
    {
        if (leftAnullable)
        {
            firstPos->insert(leftFirstPos->begin(), leftFirstPos->end());
            firstPos->insert(rightFirstPos->begin(), rightFirstPos->end());
        }
        else
        {
            firstPos->insert(leftFirstPos->begin(), leftFirstPos->end());
        }
        node->firstPos = firstPos;
        return firstPos;
    }
    else if ((wcscmp(node->value->token, L"*") == 0 || wcscmp(node->value->token, L"?") == 0 || wcscmp(node->value->token, L"+") == 0) && isLeft)
    {
        firstPos->insert(leftFirstPos->begin(), leftFirstPos->end());
        node->firstPos = firstPos;
        return firstPos;
    }
    else
    {
        node->firstPos = firstPos;
        return firstPos;
    }
}

set<wstring> *lastPosFunction(TreeNode *node)
{
    if (node == nullptr)
    {
        return nullptr;
    }
    set<wstring> *lastPos = new set<wstring>;
    bool isLeft = (node->left != nullptr);
    bool isRight = (node->right != nullptr);
    set<wstring> *leftLastPos = lastPosFunction(node->left);
    set<wstring> *rightLastPos = lastPosFunction(node->right);
    bool rightAnullable = (node->right != nullptr) ? node->right->anulable : false;

    if (node->tag != L"")
    {
        lastPos->insert(node->tag);
        node->lastPos = lastPos;
        return lastPos;
    }
    else if (wcscmp(node->value->token, L"|") == 0 && isLeft && isRight)
    {
        lastPos->insert(leftLastPos->begin(), leftLastPos->end());
        lastPos->insert(rightLastPos->begin(), rightLastPos->end());
        node->lastPos = lastPos;
        return lastPos;
    }
    else if (wcscmp(node->value->token, L".") == 0 && isLeft && isRight)
    {
        if (rightAnullable)
        {
            lastPos->insert(leftLastPos->begin(), leftLastPos->end());
            lastPos->insert(rightLastPos->begin(), rightLastPos->end());
        }
        else
        {
            lastPos->insert(rightLastPos->begin(), rightLastPos->end());
        }
        node->lastPos = lastPos;
        return lastPos;
    }
    else if ((wcscmp(node->value->token, L"*") == 0 || wcscmp(node->value->token, L"?") == 0 || wcscmp(node->value->token, L"+") == 0) && isLeft)
    {
        lastPos->insert(leftLastPos->begin(), leftLastPos->end());
        node->lastPos = lastPos;
        return lastPos;
    }
    else
    {
        node->lastPos = lastPos;
        return lastPos;
    }
}

void addNextPos(TreeNode *node, set<wstring> *leftLastPos, set<wstring> *rightFirstPos)
{
    if (node == nullptr)
    {
        return;
    }

    if (leftLastPos->find(node->tag) != leftLastPos->end())
    {
        set<wstring> *nextPos = new set<wstring>;
        if (node->nextPos == nullptr)
        {
            node->nextPos = nextPos;
        }
        node->nextPos->insert(rightFirstPos->begin(), rightFirstPos->end());
    }

    addNextPos(node->left, leftLastPos, rightFirstPos);
    addNextPos(node->right, leftLastPos, rightFirstPos);
}

set<wstring> *nextPosFunction(TreeNode *node)
{
    if (node == nullptr)
    {
        return nullptr;
    }
    set<wstring> *nextPos = new set<wstring>;
    bool isLeft = (node->left != nullptr);
    bool isRight = (node->right != nullptr);
    node->nextPos = nextPos;

    if (isLeft)
    {
        nextPosFunction(node->left);
    }
    if (isRight)
    {
        nextPosFunction(node->right);
    }

    if (wcscmp(node->value->token, L".") == 0 && isLeft && isRight)
    {
        set<wstring> *leftLastPos = node->left->lastPos;
        set<wstring> *rightFirstPos = node->right->firstPos;
        addNextPos(node, leftLastPos, rightFirstPos);
    }
    else if (wcscmp(node->value->token, L"*") == 0 && isLeft)
    {
        set<wstring> *leftLastPos = node->left->lastPos;
        set<wstring> *leftFirstPos = node->left->firstPos;

        wcout << leftFirstPos->size() << endl;
        wcout << leftLastPos->size() << endl;

        addNextPos(node, leftLastPos, leftFirstPos);
    }
    else
    {
        return nextPos;
    }
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

void printfPos(TreeNode *node)
{
    if (node == nullptr)
    {
        return;
    }

    // Imprimir la información del nodo
    wcout << L"Node value: " << node->value->token << L" ";
    wcout << L"Node tag: " << node->tag << L" ";
    wcout << L"Node firstPos: ";
    for (const auto &tag : *(node->firstPos))
    {
        wcout << tag << L" ";
    }
    wcout << endl;
    wcout << endl;

    // Recorrer los hijos del nodo
    printfPos(node->left);
    printfPos(node->right);
}

void printlPos(TreeNode *node)
{
    if (node == nullptr)
    {
        return;
    }

    // Imprimir la información del nodo
    wcout << L"Node value: " << node->value->token << L" ";
    wcout << L"Node tag: " << node->tag << L" ";
    wcout << L"Node firstPos: ";
    for (const auto &tag : *(node->lastPos))
    {
        wcout << tag << L" ";
    }
    wcout << endl;
    wcout << endl;

    // Recorrer los hijos del nodo
    printlPos(node->left);
    printlPos(node->right);
}

void printnPos(TreeNode *node)
{
    if (node == nullptr)
    {
        return;
    }

    // Imprimir la información del nodo
    wcout << L"Node value: " << node->value->token << L" ";
    wcout << L"Node tag: " << node->tag << L" ";
    wcout << L"Node nextPos: ";
    for (const auto &tag : *(node->nextPos))
    {
        wcout << tag << L" ";
    }
    wcout << endl;
    wcout << endl;

    // Recorrer los hijos del nodo
    printnPos(node->left);
    printnPos(node->right);
}

Automata *directConstruction(TreeNode *node, wstring &alphabet)
{
    Automata *automata = new Automata;

    node = tagLeaves(node);
    anullableFunction(node);
    firstPosFunction(node);
    lastPosFunction(node);

    /* printPos(node); */
    /* printlPos(node); */
    nextPosFunction(node);

    printnPos(node);
    print2DUtil(node, 0);

    return automata;
}