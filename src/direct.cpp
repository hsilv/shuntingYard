#include "direct.h"

int directCounter = 0;
wstring getDirectName()
{
    return L"a" + to_wstring(directCounter++);
}

Automata *buildDirectSnippet(TreeNode *node, wstring &alphabet, vector<Automata *> *automataList)
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
        AutomataState *end = new AutomataState;
        vector<AutomataTransition *> *startTransitions = new vector<AutomataTransition *>;
        vector<AutomataTransition *> *endTransitions = new vector<AutomataTransition *>;
        start->name = wcsdup(getDirectName().c_str());
        start->isAcceptable = false;
        end->name = wcsdup(getDirectName().c_str());
        end->isAcceptable = true;
        AutomataTransition *transition = new AutomataTransition;
        transition->from = start;
        transition->to = end;
        transition->input = node->value->token;
        startTransitions->push_back(transition);
        start->transitions = *startTransitions;
        automata->start = start;
        automata->finalStates.push_back(end);
        automata->states.push_back(start);
        automata->states.push_back(end);
        automata->transitions.insert(automata->transitions.end(), startTransitions->begin(), startTransitions->end());
    }
    else
    {
        if (wcscmp(node->value->token, L".") == 0)
        {
            Automata *right = automataList->back();
            automataList->pop_back();
            Automata *left = automataList->back();
            automataList->pop_back();

            for (AutomataTransition *transition : left->transitions)
            {
                if (std::find(left->finalStates.begin(), left->finalStates.end(), transition->to) != left->finalStates.end())
                {
                    transition->to = right->start;
                }
            }

            for (AutomataState *finalState : left->finalStates)
            {
                finalState->isAcceptable = false;

                left->states.erase(std::remove(left->states.begin(), left->states.end(), finalState), left->states.end());
            }

            left->finalStates.clear();

            left->states.insert(left->states.end(), right->states.begin(), right->states.end());

            left->transitions.insert(left->transitions.end(), right->transitions.begin(), right->transitions.end());
            left->finalStates = right->finalStates;

            automata = left;
        }
        else if (wcscmp(node->value->token, L"|") == 0)
        {
            Automata *right = automataList->back();
            automataList->pop_back();
            Automata *left = automataList->back();
            automataList->pop_back();

            AutomataState *start = new AutomataState;
            vector<AutomataTransition *> *startTransitions = new vector<AutomataTransition *>;
            start->name = wcsdup(getDirectName().c_str());
            start->isAcceptable = false;

            for (AutomataTransition *transition : left->start->transitions)
            {
                AutomataTransition *newTransition = new AutomataTransition;
                newTransition->from = start;
                newTransition->to = transition->to;
                newTransition->input = transition->input;
                startTransitions->push_back(newTransition);
            }

            for (AutomataTransition *transition : right->start->transitions)
            {
                AutomataTransition *newTransition = new AutomataTransition;
                newTransition->from = start;
                newTransition->to = transition->to;
                newTransition->input = transition->input;
                startTransitions->push_back(newTransition);
            }

            for (AutomataTransition *transition : left->transitions)
            {
                if (transition->to == left->start)
                {
                    transition->to = start;
                }
            }

            for (AutomataTransition *transition : right->transitions)
            {
                if (transition->to == right->start)
                {
                    transition->to = start;
                }
            }

            left->states.erase(std::remove(left->states.begin(), left->states.end(), left->start), left->states.end());

            right->states.erase(std::remove(right->states.begin(), right->states.end(), right->start), right->states.end());

            left->transitions.erase(std::remove_if(left->transitions.begin(), left->transitions.end(),
                                                   [left](AutomataTransition *transition)
                                                   {
                                                       return transition->from == left->start || transition->to == left->start;
                                                   }),
                                    left->transitions.end());

            right->transitions.erase(std::remove_if(right->transitions.begin(), right->transitions.end(),
                                                    [right](AutomataTransition *transition)
                                                    {
                                                        return transition->from == right->start || transition->to == right->start;
                                                    }),
                                     right->transitions.end());

            start->transitions = *startTransitions;
            automata->start = start;
            automata->states.insert(automata->states.end(), left->states.begin(), left->states.end());
            automata->states.insert(automata->states.end(), right->states.begin(), right->states.end());
            automata->finalStates.insert(automata->finalStates.end(), left->finalStates.begin(), left->finalStates.end());
            automata->finalStates.insert(automata->finalStates.end(), right->finalStates.begin(), right->finalStates.end());
            automata->transitions.insert(automata->transitions.end(), startTransitions->begin(), startTransitions->end());
            automata->transitions.insert(automata->transitions.end(), left->transitions.begin(), left->transitions.end());
            automata->transitions.insert(automata->transitions.end(), right->transitions.begin(), right->transitions.end());
        }
        else if (wcscmp(node->value->token, L"*") == 0)
        {
            Automata *left = automataList->back();
            automataList->pop_back();

            AutomataTransition *transition = new AutomataTransition;
        }
    }
    return automata;
}

Automata *directConstruction(TreeNode *node, wstring &alphabet)
{
    if (node == nullptr)
    {
        return nullptr;
    }

    vector<Automata *> *automataList = new vector<Automata *>;

    Automata *left = directConstruction(node->left, alphabet);
    Automata *right = directConstruction(node->right, alphabet);

    if (left != nullptr)
    {
        automataList->push_back(left);
    }

    if (right != nullptr)
    {
        automataList->push_back(right);
    }

    Automata *current = buildDirectSnippet(node, alphabet, automataList);

    return current;
}