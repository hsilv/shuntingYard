#include "direct.h"
#include <vector>

static int tagCounter = 1;
static int stateCounter = 0;
wstring generateTag()
{
    return to_wstring(tagCounter++);
}

wstring generateDirectState()
{
    return L"q" + to_wstring(stateCounter++);
}

struct AlphabetTransition;
struct TransitionItem
{
    set<wstring> *positions;
    wstring stateName;
    vector<AlphabetTransition *> *transitions;
    bool isInitial;
    bool isFinal;
};
struct AlphabetTransition
{
    TransitionItem *to;
    wchar_t *input;
    TransitionItem *from;
};

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
    else if ((wcscmp(node->value->token, L"*") == 0 || wcscmp(node->value->token, L"+") == 0 || wcscmp(node->value->token, L"?") == 0) && isLeft)
    {
        set<wstring> *leftLastPos = node->left->lastPos;
        set<wstring> *leftFirstPos = node->left->firstPos;

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

    wcout << L"Node value: " << node->value->token << L" ";
    wcout << L"Node tag: " << node->tag << L" ";
    wcout << L"Node firstPos: ";
    for (const auto &tag : *(node->firstPos))
    {
        wcout << tag << L" ";
    }
    wcout << endl;
    wcout << endl;

    printfPos(node->left);
    printfPos(node->right);
}

void printlPos(TreeNode *node)
{
    if (node == nullptr)
    {
        return;
    }

    wcout << L"Node value: " << node->value->token << L" ";
    wcout << L"Node tag: " << node->tag << L" ";
    wcout << L"Node firstPos: ";
    for (const auto &tag : *(node->lastPos))
    {
        wcout << tag << L" ";
    }
    wcout << endl;
    wcout << endl;

    printlPos(node->left);
    printlPos(node->right);
}

void printnPos(TreeNode *node)
{
    if (node == nullptr)
    {
        return;
    }

    wcout << L"Node value: " << node->value->token << L" ";
    wcout << L"Node tag: " << node->tag << L" ";
    wcout << L"Node nextPos: ";
    for (const auto &tag : *(node->nextPos))
    {
        wcout << tag << L" ";
    }
    wcout << endl;
    wcout << endl;

    printnPos(node->left);
    printnPos(node->right);
}

TreeNode *findNodeByTag(TreeNode *node, const wstring &tag)
{
    if (node == nullptr)
    {
        return nullptr;
    }

    if (node->tag == tag)
    {
        return node;
    }

    TreeNode *leftSearch = findNodeByTag(node->left, tag);
    if (leftSearch != nullptr)
    {
        return leftSearch;
    }

    TreeNode *rightSearch = findNodeByTag(node->right, tag);
    if (rightSearch != nullptr)
    {
        return rightSearch;
    }

    return nullptr;
}

void findNodesWithoutTagAndEmptyNextPos(TreeNode *node, vector<TreeNode *> &nodes)
{
    if (node == nullptr)
    {
        return;
    }

    if (!node->tag.empty() && (node->nextPos == nullptr || node->nextPos->empty()))
    {
        nodes.push_back(node);
    }

    findNodesWithoutTagAndEmptyNextPos(node->left, nodes);
    findNodesWithoutTagAndEmptyNextPos(node->right, nodes);
}

void findNodesByTagAndToken(TreeNode *node, const wstring &tag, const wstring &token, set<TreeNode *> &nodes)
{
    if (node == nullptr)
    {
        return;
    }

    if (node->tag == tag && node->value->token == token)
    {
        nodes.insert(node);
    }

    findNodesByTagAndToken(node->left, tag, token, nodes);
    findNodesByTagAndToken(node->right, tag, token, nodes);
}

AutomataState *findStateByName(std::vector<AutomataState *> states, const std::wstring &name)
{
    for (AutomataState *state : states)
    {
        if (state->name == name)
        {
            return state;
        }
    }
    return nullptr; // return null if no state with the given name is found
}

Automata *directConstruction(TreeNode *node, wstring &alphabet)
{
    Automata *automata = new Automata;
    vector<AutomataState *> states;
    AutomataState *start = new AutomataState;

    node = tagLeaves(node);
    anullableFunction(node);
    firstPosFunction(node);
    lastPosFunction(node);

    nextPosFunction(node);

    TreeNode *initialNode = findNodeByTag(node, L"1");

    vector<TreeNode *> finalStates;
    findNodesWithoutTagAndEmptyNextPos(node, finalStates);

    if (initialNode == nullptr || finalStates.empty())
    {
        throw runtime_error("No se encontró el nodo inicial o los nodos finales");
    }

    vector<TransitionItem *> transitionTable;
    vector<AlphabetTransition *> allTransitions;

    TransitionItem *initialTransition = new TransitionItem;

    initialTransition->positions = initialNode->nextPos;
    initialTransition->stateName = generateDirectState();
    initialTransition->transitions = new vector<AlphabetTransition *>;
    transitionTable.push_back(initialTransition);

    bool newStateAdded;
    do
    {
        newStateAdded = false;
        int transitionTableSize = transitionTable.size();
        for (int i = 0; i < transitionTableSize; i++)
        {
            TransitionItem *transition = transitionTable[i];
            for (const auto &symbol : alphabet)
            {

                set<TreeNode *> nodes;
                set<wstring> *subsetTags = new set<wstring>;
                /* wcout << L"Searching for tag:"; */

                for (const auto &item : *transition->positions)
                {
                    findNodesByTagAndToken(node, item, wstring(1, symbol), nodes);
                    /* wcout << L" " << item; */
                }
                /* wcout << L" and token: " << wstring(1, symbol); */

                for (const auto &item : nodes)
                {
                    subsetTags->insert(item->nextPos->begin(), item->nextPos->end());
                }

                /* wcout << L" gives: { ";
                for (const auto &tag : *subsetTags)
                {
                    wcout << tag << L", ";
                }
                wcout << L"} " << endl; */

                bool exists = false;
                TransitionItem *repeated = nullptr;
                for (const auto &item : transitionTable)
                {
                    if (*item->positions == *subsetTags)
                    {
                        exists = true;
                        repeated = item;
                        break;
                    }
                }

                if (!exists && !subsetTags->empty())
                {
                    TransitionItem *newTransition = new TransitionItem;
                    newTransition->positions = subsetTags;
                    newTransition->stateName = generateDirectState();
                    newTransition->transitions = new vector<AlphabetTransition *>;
                    AlphabetTransition *newAlphabetTransition = new AlphabetTransition;
                    newAlphabetTransition->to = newTransition;
                    newAlphabetTransition->from = transition;
                    newAlphabetTransition->input = wcsdup(&symbol);
                    transition->transitions->push_back(newAlphabetTransition);
                    allTransitions.push_back(newAlphabetTransition);
                    transitionTable.push_back(newTransition);
                    newStateAdded = true;
                    transitionTableSize++;
                }
                else if (exists)
                {
                    /* wcout << "Adding transition to existing state" << endl; */
                    AlphabetTransition *newAlphabetTransition = new AlphabetTransition;
                    newAlphabetTransition->to = repeated;
                    newAlphabetTransition->from = transition;
                    newAlphabetTransition->input = wcsdup(wstring(1, symbol).c_str());

                    auto it = std::find_if(transition->transitions->begin(), transition->transitions->end(),
                                           [newAlphabetTransition](const AlphabetTransition *at)
                                           {
                                               return at->to == newAlphabetTransition->to &&
                                                      at->from == newAlphabetTransition->from &&
                                                      *at->input == *newAlphabetTransition->input;
                                           });

                    if (it == transition->transitions->end())
                    {
                        transition->transitions->push_back(newAlphabetTransition);
                        allTransitions.push_back(newAlphabetTransition);
                    }
                }
            }
        }
    } while (newStateAdded);

    /* for (int i = 0; i < transitionTable.size(); i++)
    {
        wcout << transitionTable[i]->stateName << L" ";
        wcout << "items: ";
        for (const auto &item : *transitionTable[i]->positions)
        {
            wcout << item << L" ";
        }
        wcout << L"transitions: ";
        for (const auto &item : *transitionTable[i]->transitions)
        {
            wcout << item->from->stateName << L" --" << item->input[0] << L"-> " << item->to->stateName << L" ";
        }
        wcout << endl;
    } */

    for (int i = 0; i < transitionTable.size(); i++)
    {
        if (*(transitionTable[i]->positions) == *(initialNode->nextPos))
        {
            initialTransition->isInitial = true;
        }
        if (finalStates.size() > 0)
        {
            for (int j = 0; j < finalStates.size(); j++)
            {
                if (transitionTable[i]->positions->find(finalStates[j]->tag) != transitionTable[i]->positions->end())
                {
                    transitionTable[i]->isFinal = true;
                }
            }
        }
    }

    for (int i = 0; i < transitionTable.size(); i++)
    {
        wcout << transitionTable[i]->stateName << L" ";
        wcout << "items: ";
        for (const auto &item : *transitionTable[i]->positions)
        {
            wcout << item << L" ";
        }
        wcout << L"transitions: ";
        for (const auto &item : *transitionTable[i]->transitions)
        {
            wcout << item->from->stateName << L" --" << item->input[0] << L"-> " << item->to->stateName << L" ";
        }
        wcout << endl;

        wcout << L"isInitial: ";

        wcout << transitionTable[i]->isInitial << endl;

        wcout << L"isFinal: ";

        wcout << transitionTable[i]->isFinal << endl;
    }

    for (int i = 0; i < transitionTable.size(); i++)
    {
        AutomataState *state = new AutomataState;
        state->name = wcsdup(wstring(transitionTable[i]->stateName).c_str());
        wcout << state->name << endl;
        state->isAcceptable = transitionTable[i]->isFinal;
        automata->states.push_back(state);
        if (state->isAcceptable)
        {
            automata->finalStates.push_back(state);
        }
        if (transitionTable[i]->isInitial == 1)
        {
            start->name = wcsdup(state->name);
            start->isAcceptable = state->isAcceptable;
        }
    }

    for (int i = 0; i < allTransitions.size(); i++)
    {
        AlphabetTransition *at = allTransitions[i];
        AutomataState *from = findStateByName(automata->states, at->from->stateName);
        AutomataState *to = findStateByName(automata->states, at->to->stateName);
        AutomataTransition *transition = new AutomataTransition;
        transition->from = from;
        transition->to = to;
        transition->input = wcsdup(wstring(1, *at->input).c_str());
        automata->transitions.push_back(transition);
        from->transitions.push_back(transition);
        to->transitions.push_back(transition);
    }

    automata->alphabet = alphabet;

    automata->start = start;

    wcout << automata->start->name << endl;

    print2DUtil(node, 0);

    return automata;
}