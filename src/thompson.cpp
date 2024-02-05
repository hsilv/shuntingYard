#include "thompson.h"
#include "tree.h"

static int counter = 0;

wstring getHashName()
{
    return L"q" + to_wstring(counter++);
}

Automata *buildThompsonSnippet(TreeNode *node, const wstring &alphabet, vector<Automata *> *automataList)
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
        start->isAcceptable = false;
        automata->start = start;
        start->name = wcsdup(getHashName().c_str());

        AutomataState *end = new AutomataState;
        end->isAcceptable = true;
        end->name = wcsdup(getHashName().c_str());

        AutomataTransition *transition = new AutomataTransition;
        transition->from = start;
        transition->to = end;
        transition->input = node->value->token;

        automata->states.push_back(start);
        automata->states.push_back(end);
        automata->transitions.push_back(transition);
        automata->finalStates.push_back(end);
    }
    else
    {
        if (wcscmp(node->value->token, L".") == 0)
        {
            wcout << "Concatenation" << endl;
            Automata *right = automataList->back();
            automataList->pop_back();
            Automata *left = automataList->back();
            automataList->pop_back();

            for (int i = 0; i < left->finalStates.size(); i++)
            {
                left->finalStates[i]->isAcceptable = false;
                for (int j = 0; j < left->transitions.size(); j++)
                {
                    if (left->transitions[j]->to == left->finalStates[i])
                    {
                        left->transitions[j]->to = right->start;
                    }
                }
            }

            left->finalStates = right->finalStates;
            left->states.insert(left->states.end(), right->states.begin(), right->states.end());
            left->transitions.insert(left->transitions.end(), right->transitions.begin(), right->transitions.end());

            automata->finalStates = left->finalStates;
            automata->start = left->start;
            automata->states = left->states;
            automata->transitions = left->transitions;
        }
        else if (wcscmp(node->value->token, L"|") == 0)
        {
            wcout << "Union" << endl;
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
            AutomataTransition *toERight = new AutomataTransition;
            AutomataTransition *toELeft = new AutomataTransition;

            toSRight->from = start;
            toSRight->to = right->start;
            toSRight->input = L"ε";

            toSLeft->from = start;
            toSLeft->to = left->start;
            toSLeft->input = L"ε";

            for (int i = 0; i < right->finalStates.size(); i++)
            {
                right->finalStates[i]->isAcceptable = false;
                toERight->from = right->finalStates[i];
                toERight->to = end;
                toERight->input = L"ε";
            }

            for (int i = 0; i < left->finalStates.size(); i++)
            {
                left->finalStates[i]->isAcceptable = false;
                toELeft->from = left->finalStates[i];
                toELeft->to = end;
                toELeft->input = L"ε";
            }

            automata->states.push_back(start);
            automata->states.push_back(end);
            automata->finalStates.push_back(end);

            left->states.insert(left->states.end(), right->states.begin(), right->states.end());
            left->transitions.insert(left->transitions.end(), right->transitions.begin(), right->transitions.end());

            automata->start = start;
            automata->states = left->states;
            automata->transitions = left->transitions;
            automata->transitions.push_back(toSRight);
            automata->transitions.push_back(toSLeft);
            automata->transitions.push_back(toERight);
            automata->transitions.push_back(toELeft);
        }
        else if (wcscmp(node->value->token, L"*") == 0 || wcscmp(node->value->token, L"+") == 0)
        {

            Automata *left = automataList->back();
            wcout << left->start->name << endl;
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
            automata->transitions.push_back(toS);

            for (int i = 0; i < left->finalStates.size(); i++)
            {
                left->finalStates[i]->isAcceptable = false;
                AutomataTransition *toE = new AutomataTransition;
                AutomataTransition *EtoS = new AutomataTransition;
                EtoS->from = left->finalStates[i];
                EtoS->to = left->start;
                EtoS->input = L"ε";
                automata->transitions.push_back(EtoS);
                toE->from = left->finalStates[i];
                toE->to = end;
                toE->input = L"ε";
                automata->transitions.push_back(toE);
            }

            if (wcscmp(node->value->token, L"*") == 0)
            {
                AutomataTransition *StoE = new AutomataTransition;
                StoE->from = start;
                StoE->to = end;
                StoE->input = L"ε";
                automata->transitions.push_back(StoE);
            }

            automata->finalStates.push_back(end);
        }
    }

    return automata;
}

Automata *thompson(TreeNode *node, const wstring &alphabet)
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

    if (current != nullptr)
    {
        wcout << L"Automata: " << current->start->name << endl;
    }

    for (int i = 0; i < current->transitions.size(); i++)
    {
        wcout << L"Transition: " << current->transitions[i]->from->name << L" -> " << current->transitions[i]->to->name << L" with " << current->transitions[i]->input << endl;
    }

    return current;
}