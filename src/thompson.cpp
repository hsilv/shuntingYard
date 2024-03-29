#include "thompson.h"
#include "tree.h"

static int counter = 0;

wstring getHashName()
{
    return L"q" + to_wstring(counter++);
}

void addEpsilon(wstring &alphabet)
{
    if (!wcschr(alphabet.c_str(), L'ε'))
    {
        alphabet += L'ε';
    }
}

Automata *buildThompsonSnippet(TreeNode *node, wstring &alphabet, vector<Automata *> *automataList)
{
    Automata *automata = new Automata;

    vector<AutomataState *> *states = new vector<AutomataState *>;
    automata->states = *states;

    vector<AutomataState *> *finalStates = new vector<AutomataState *>;
    automata->finalStates = *finalStates;

    vector<AutomataTransition *> *transitions = new vector<AutomataTransition *>;
    automata->transitions = *transitions;

    automata->alphabet = alphabet;

    if (node->value->type == shuntingToken::OPERAND)
    {
        AutomataState *start = new AutomataState;
        vector<AutomataTransition *> *startTransitions = new vector<AutomataTransition *>;
        start->isAcceptable = false;
        start->name = wcsdup(getHashName().c_str());

        AutomataState *end = new AutomataState;
        vector<AutomataTransition *> *endTransitions = new vector<AutomataTransition *>;
        end->isAcceptable = true;
        end->name = wcsdup(getHashName().c_str());

        AutomataTransition *transition = new AutomataTransition;
        transition->from = start;
        transition->to = end;
        transition->input = node->value->token;

        startTransitions->push_back(transition);
        start->transitions = *startTransitions;
        end->transitions = *endTransitions;

        automata->start = start;
        automata->states.push_back(start);
        automata->states.push_back(end);
        automata->transitions.push_back(transition);
        automata->finalStates.push_back(end);
    }
    else
    {
        if (wcscmp(node->value->token, L".") == 0)
        {
            Automata *right = automataList->back();
            automataList->pop_back();
            Automata *left = automataList->back();
            automataList->pop_back();

            for (int i = 0; i < left->finalStates.size(); i++)
            {
                left->finalStates[i]->isAcceptable = false;
                AutomataTransition *transition = new AutomataTransition;
                transition->from = left->finalStates[i];
                transition->to = right->start;
                transition->input = L"ε";
                left->finalStates[i]->transitions.push_back(transition);
                left->transitions.push_back(transition);
            }

            left->finalStates = right->finalStates;
            left->states.insert(left->states.end(), right->states.begin(), right->states.end());
            left->transitions.insert(left->transitions.end(), right->transitions.begin(), right->transitions.end());

            for (int i = 0; i < left->states.size(); i++)
            {
                for (int j = 0; j < left->states[i]->transitions.size(); j++)
                {
                    if (left->states[i]->transitions[j]->to == left->states[i])
                    {
                        left->states[i]->transitions[j]->to = right->start;
                    }
                }
            }

            for (int i = 0; i < left->finalStates.size(); i++)
            {
                for (int j = 0; j < left->finalStates[i]->transitions.size(); j++)
                {
                    if (left->finalStates[i]->transitions[j]->to == left->finalStates[i])
                    {
                        left->finalStates[i]->transitions[j]->to = right->start;
                    }
                }
            }

            automata->finalStates = left->finalStates;
            automata->start = left->start;
            automata->states = left->states;
            automata->transitions = left->transitions;
        }
        else if (wcscmp(node->value->token, L"|") == 0)
        {
            Automata *right = automataList->back();
            automataList->pop_back();
            Automata *left = automataList->back();
            automataList->pop_back();

            AutomataState *start = new AutomataState;
            start->isAcceptable = false;
            start->name = wcsdup(getHashName().c_str());

            AutomataState *end = new AutomataState;
            end->isAcceptable = true;
            end->name = wcsdup(getHashName().c_str());

            AutomataTransition *toSRight = new AutomataTransition;
            AutomataTransition *toSLeft = new AutomataTransition;

            toSRight->from = start;
            toSRight->to = right->start;
            toSRight->input = L"ε";
            start->transitions.push_back(toSRight);

            toSLeft->from = start;
            toSLeft->to = left->start;
            toSLeft->input = L"ε";
            start->transitions.push_back(toSLeft);

            for (int i = 0; i < right->finalStates.size(); i++)
            {
                AutomataTransition *toERight = new AutomataTransition;
                right->finalStates[i]->isAcceptable = false;
                toERight->from = right->finalStates[i];
                toERight->to = end;
                toERight->input = L"ε";
                right->finalStates[i]->transitions.push_back(toERight);
                left->transitions.push_back(toERight);
            }

            for (int i = 0; i < left->finalStates.size(); i++)
            {
                AutomataTransition *toELeft = new AutomataTransition;
                left->finalStates[i]->isAcceptable = false;
                toELeft->from = left->finalStates[i];
                toELeft->to = end;
                toELeft->input = L"ε";
                left->finalStates[i]->transitions.push_back(toELeft);
                left->transitions.push_back(toELeft);
            }

            left->states.insert(left->states.end(), right->states.begin(), right->states.end());
            left->transitions.insert(left->transitions.end(), right->transitions.begin(), right->transitions.end());

            automata->start = start;
            automata->states = left->states;
            automata->transitions = left->transitions;
            automata->transitions.push_back(toSRight);
            automata->transitions.push_back(toSLeft);
            addEpsilon(alphabet);
            automata->alphabet = alphabet;
            automata->states.push_back(start);
            automata->states.push_back(end);
            automata->finalStates.push_back(end);
        }
        else if (wcscmp(node->value->token, L"*") == 0 || wcscmp(node->value->token, L"+") == 0)
        {

            Automata *left = automataList->back();
            automataList->pop_back();
            automata->states.insert(automata->states.end(), left->states.begin(), left->states.end());
            automata->transitions.insert(automata->transitions.end(), left->transitions.begin(), left->transitions.end());

            AutomataState *start = new AutomataState;
            start->isAcceptable = false;
            start->name = wcsdup(getHashName().c_str());
            automata->start = start;

            AutomataState *end = new AutomataState;
            end->isAcceptable = true;
            end->name = wcsdup(getHashName().c_str());

            AutomataTransition *toS = new AutomataTransition;
            toS->from = start;
            toS->to = left->start;
            toS->input = L"ε";
            start->transitions.push_back(toS);
            automata->transitions.push_back(toS);

            for (int i = 0; i < left->finalStates.size(); i++)
            {
                left->finalStates[i]->isAcceptable = false;
                AutomataTransition *toE = new AutomataTransition;
                AutomataTransition *EtoS = new AutomataTransition;
                EtoS->from = left->finalStates[i];
                EtoS->to = left->start;
                EtoS->input = L"ε";
                left->finalStates[i]->transitions.push_back(EtoS);
                automata->transitions.push_back(EtoS);
                toE->from = left->finalStates[i];
                toE->to = end;
                toE->input = L"ε";
                left->finalStates[i]->transitions.push_back(toE);
                automata->transitions.push_back(toE);
            }

            if (wcscmp(node->value->token, L"*") == 0)
            {
                AutomataTransition *StoE = new AutomataTransition;
                StoE->from = start;
                StoE->to = end;
                StoE->input = L"ε";
                start->transitions.push_back(StoE);
                automata->transitions.push_back(StoE);
            }

            addEpsilon(alphabet);
            automata->alphabet = alphabet;
            automata->states.push_back(start);
            automata->states.push_back(end);
            automata->finalStates.push_back(end);
        }
        else if (wcscmp(node->value->token, L"?") == 0)
        {
            Automata *left = automataList->back();
            automata = left;
            automata->start->isAcceptable = false;

            for (int i = 0; i < automata->finalStates.size(); i++)
            {
                automata->finalStates[i]->isAcceptable = true;
                AutomataTransition *toE = new AutomataTransition;
                toE->from = automata->start;
                toE->to = automata->finalStates[i];
                toE->input = L"ε";
                automata->start->transitions.push_back(toE);
                automata->transitions.push_back(toE);
            }

            addEpsilon(alphabet);
            automata->alphabet = alphabet;
        }
    }

    return automata;
}

Automata *thompson(TreeNode *node, wstring &alphabet)
{
    if (node == nullptr)
    {
        return nullptr;
    }

    vector<Automata *> *automataList = new vector<Automata *>;

    Automata *left = thompson(node->left, alphabet);
    Automata *right = thompson(node->right, alphabet);

    if (left != nullptr)
    {
        automataList->push_back(left);
    }

    if (right != nullptr)
    {
        automataList->push_back(right);
    }

    Automata *current = buildThompsonSnippet(node, alphabet, automataList);

    return current;
}