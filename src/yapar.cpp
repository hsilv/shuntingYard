#include "yapar.h"
#include <set>
#include <algorithm>

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