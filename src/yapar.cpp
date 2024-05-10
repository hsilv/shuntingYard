#include "yapar.h"
#include <set>
#include <algorithm>
#include <iostream>

using namespace std;

Grammar augment(const Grammar &grammar)
{
    GrammarToken dot;
    dot.type = Dot;
    dot.value = L".";
    Grammar augmentedGrammar = grammar;
    GrammarToken start;
    start.type = NonTerminal;
    start.value = L"S'";
    augmentedGrammar.start = start;
    GrammarProduction newProd;
    newProd.left = start;
    newProd.right.push_back(dot);
    newProd.right.push_back(grammar.start);
    newProd.type = Core;
    augmentedGrammar.productions.insert(augmentedGrammar.productions.begin(), newProd);
    augmentedGrammar.nonTerminals.insert(start);
    return augmentedGrammar;
}

set<GrammarProduction> closure(const Grammar &grammar, const set<GrammarProduction> &I)
{
    set<GrammarProduction> J = I;
    bool changed = true;

    while (changed)
    {
        changed = false;
        for (GrammarProduction prod : J)
        {
            vector<GrammarToken>::iterator dotPos;
            for (dotPos = prod.right.begin(); dotPos != prod.right.end(); ++dotPos)
            {
                if (dotPos->type == Dot)
                {
                    break;
                }
            }

            if (dotPos != prod.right.end() && dotPos + 1 != prod.right.end() && (dotPos + 1)->type == NonTerminal)
            {
                GrammarToken B = *(dotPos + 1);
                for (GrammarProduction prod2 : grammar.productions)
                {
                    if (prod2.left.value == B.value)
                    {
                        GrammarProduction newProd = prod2;
                        GrammarToken dot;
                        dot.type = Dot;
                        dot.value = L".";
                        newProd.right.insert(newProd.right.begin(), dot);
                        newProd.type = NoCore;
                        if (J.find(newProd) == J.end())
                        {
                            J.insert(newProd);
                            changed = true;
                        }
                    }
                }
            }
        }
    }
    return J;
}

set<GrammarProduction> gotoSet(const Grammar &grammar, const set<GrammarProduction> &I, const GrammarToken &X)
{
    set<GrammarProduction> J;
    for (const GrammarProduction &prod : I)
    {
        // Encuentra el punto una vez
        auto dotPos = find_if(prod.right.begin(), prod.right.end(), [](const GrammarToken &token)
                              { return token.type == Dot; });

        if (dotPos != prod.right.end() && dotPos + 1 != prod.right.end() && (dotPos + 1)->value == X.value)
        {
            GrammarProduction newProd = prod;
            // Crea una nueva producción con el punto movido a la derecha
            iter_swap(newProd.right.begin() + (dotPos - prod.right.begin()), newProd.right.begin() + (dotPos - prod.right.begin() + 1));
            newProd.type = Core;
            J.insert(newProd);
        }
    }
    return closure(grammar, J);
}

LR0Automata build(const Grammar &grammar)
{
    set<set<GrammarProduction>> C;
    set<GrammarProduction> I0 = closure(grammar, {grammar.productions[0]});
    C.insert(I0);

    map<set<GrammarProduction>, wstring> setsToName;
    map<wstring, set<GrammarProduction>> nameToSets;
    map<wstring, map<GrammarToken, wstring>> transitions;
    vector<wstring> acceptanceStates;
    LR0Automata automata;

    int stateCounter = 0;

    bool changed = true;

    while (changed)
    {
        changed = false;
        for (set<GrammarProduction> I : C)
        {
            if (setsToName.find(I) == setsToName.end())
            {
                wstring name = L"I" + to_wstring(stateCounter++);
                setsToName[I] = name;
                nameToSets[name] = I;
            }

            wstring currentStateName = setsToName[I];

            for (GrammarToken token : grammar.terminals)
            {
                set<GrammarProduction> J = gotoSet(grammar, I, token);
                if (J.size() > 0)
                {
                    if (C.find(J) == C.end())
                    {
                        C.insert(J);
                        changed = true;

                        if (setsToName.find(J) == setsToName.end())
                        {
                            wstring name = L"I" + to_wstring(stateCounter++);
                            setsToName[J] = name;
                            nameToSets[name] = J;
                        }
                    }

                    wstring nextStateName = setsToName[J];

                    // Agregar transición
                    transitions[currentStateName][token] = nextStateName;
                }
            }

            for (GrammarToken token : grammar.nonTerminals)
            {
                set<GrammarProduction> J = gotoSet(grammar, I, token);
                if (J.size() > 0)
                {
                    if (C.find(J) == C.end())
                    {
                        C.insert(J);
                        changed = true;

                        if (setsToName.find(J) == setsToName.end())
                        {
                            wstring name = L"I" + to_wstring(stateCounter++);
                            setsToName[J] = name;
                            nameToSets[name] = J;
                        }
                    }

                    wstring nextStateName = setsToName[J];

                    // Agregar transición
                    transitions[currentStateName][token] = nextStateName;
                }
            }
        }
    }

    vector<LR0AutomataState> states;
    // Imprimir los sets y encontrar estados de aceptación
    for (const auto &pair : setsToName)
    {
        const set<GrammarProduction> &productions = pair.first;
        const wstring &name = pair.second;

        wcout << name << ":\n";
        bool isAcceptanceState = false;
        for (const GrammarProduction &production : productions)
        {
            wcout << L"  " << production.left.value << L" -> ";
            for (const GrammarToken &token : production.right)
            {
                wcout << token.value << L" ";
            }
            wcout << "\n";

            if (production.left.value == grammar.productions[0].left.value &&
                production.right.back().value == L".")
            {
                isAcceptanceState = true;
            }
        }

        if (isAcceptanceState)
        {
            acceptanceStates.push_back(name);
        }

        // Agregar estado a la lista de estados
        LR0AutomataState state;
        state.name = name;
        state.productions = productions;
        states.push_back(state);
    }

    wcout << "\n";

    // Imprimir las transiciones
    wcout << L"Transiciones:\n";
    for (const auto &pair : transitions)
    {
        const wstring &fromName = pair.first;
        const map<GrammarToken, wstring> &toStates = pair.second;

        for (const auto &toPair : toStates)
        {
            const GrammarToken &token = toPair.first;
            const wstring &toName = toPair.second;

            wcout << fromName << L"-" << token.value << L"->" << toName << "\n";
        }
    }

    automata.start = &states[0];
    automata.states = states;
    automata.transitions = transitions;
    automata.acceptanceStates = acceptanceStates;

    // Imprimir los estados de aceptación
    wcout << L"Estados de aceptación:\n";
    for (const wstring &state : acceptanceStates)
    {
        wcout << state << "\n";
    }

    return automata;
}

set<GrammarToken> first(const Grammar &grammar, const GrammarToken &token, set<wstring> &visited)
{
    set<GrammarToken> result;

    if (token.type == Terminal)
    {
        result.insert(token);
    }
    else if (token.type == NonTerminal)
    {
        // Evitar la recursión infinita
        if (visited.find(token.value) != visited.end())
        {
            return result;
        }
        visited.insert(token.value);

        for (GrammarProduction production : grammar.productions)
        {
            if (production.left.value == token.value)
            {
                if (!production.right.empty())
                {
                    if (production.right[0].type == Terminal)
                    {
                        result.insert(production.right[0]);
                    }
                    else if (production.right[0].type == NonTerminal)
                    {
                        set<GrammarToken> firstSet = first(grammar, production.right[0], visited);
                        result.insert(firstSet.begin(), firstSet.end());
                    }
                }
            }
        }
    }
    return result;
}

set<GrammarToken> next(const Grammar &grammar, const GrammarToken &token)
{
    set<GrammarToken> result;

    // Si el token es igual al lado izquierdo de la primera producción, agregar "$"
    if (grammar.productions[0].left.value == token.value)
    {
        GrammarToken dollar;
        dollar.type = Special;
        dollar.value = L"$";
        result.insert(dollar);
    }

    if (token.type == Terminal)
    {
        return result;
    }
    else if (token.type == NonTerminal)
    {
        for (GrammarProduction production : grammar.productions)
        {
            for (int i = 0; i < production.right.size(); i++)
            {
                if (production.right[i].value == token.value)
                {
                    if (i + 1 < production.right.size())
                    {
                        // Agregar el conjunto first del token a la derecha del token actual al conjunto next
                        set<wstring> visited;
                        set<GrammarToken> firstSet = first(grammar, production.right[i + 1], visited);
                        result.insert(firstSet.begin(), firstSet.end());
                    }
                    else
                    {
                        // Si el token es el último en la producción, añadir el conjunto next del lado izquierdo de la producción
                        set<GrammarToken> nextSet = next(grammar, production.left);
                        result.insert(nextSet.begin(), nextSet.end());
                    }
                }
            }
        }
    }
    return result;
}