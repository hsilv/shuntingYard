#ifndef YAPAR_H
#define YAPAR_H

#include <set>
#include <vector>
#include <string>
#include <map>

using namespace std;

enum GrammarTokenType
{
    Terminal,
    NonTerminal,
    Dot,
    Special
};

enum GrammarProductionType
{
    Core,
    NoCore
};

struct GrammarToken
{
    GrammarTokenType type;
    wstring value;
    bool operator<(const GrammarToken &other) const
    {
        if (value < other.value)
            return true;
        if (value > other.value)
            return false;
        return type < other.type;
    }
    bool operator!=(const GrammarToken &other) const
    {
        // Define cómo comparar dos objetos de la clase GrammarToken
        // Por ejemplo, puedes comparar primero los tipos, y si son iguales, comparar los valores
        return !(*this == other);
    }
    bool operator==(const GrammarToken &other) const
    {
        // Define cómo comparar dos objetos de la clase GrammarToken
        // Por ejemplo, puedes comparar primero los tipos, y si son iguales, comparar los valores
        return type == other.type && value == other.value;
    }
}

;

struct GrammarProduction
{
    GrammarToken left;
    vector<GrammarToken> right;
    GrammarProductionType type;
    bool operator<(const GrammarProduction &other) const
    {
        if (left != other.left)
        {
            return left < other.left;
        }
        // Compara los vectores 'right' elemento por elemento
        return lexicographical_compare(right.begin(), right.end(), other.right.begin(), other.right.end());
    }
};

struct Grammar
{
    vector<GrammarProduction> productions;
    set<GrammarToken> terminals;
    set<GrammarToken> nonTerminals;
    GrammarToken start;
};

struct LR0AutomataState
{
    wstring name;
    set<GrammarProduction> productions;
};

struct LR0Automata
{
    vector<LR0AutomataState> states;
    map<wstring, map<GrammarToken, wstring>> transitions;
    vector<wstring> acceptanceStates;
    LR0AutomataState *start;
};

Grammar augment(const Grammar &grammar);
set<GrammarProduction> closure(const Grammar &grammar, const set<GrammarProduction> &I);
set<GrammarProduction> gotoSet(const Grammar &grammar, const set<GrammarProduction> &I, const GrammarToken &X);
LR0Automata build(const Grammar &grammar);
set<GrammarToken> next(const Grammar &grammar, const GrammarToken &token, set<wstring> &visited);
set<GrammarToken> first(const Grammar &grammar, const GrammarToken &token, set<wstring> &visited);

#endif