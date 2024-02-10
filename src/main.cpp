#include "shunting.h"
#include <chrono>
#include <iostream>
#include <thread>
#include "automata.h"
#include "tree.h"
#include "thompson.h"
#include "graphviz.h"
#include "subsets.h"
#include "direct.h"
#include "minification.h"
#include "simulation.h"

Stack<shuntingToken> postfix;
Stack<shuntingToken> postfixAugmented;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "\033[1;31m"
             << "ERROR: Debe ingresar una expresión regular y una cadena a validar"
             << "\033[0m" << endl;
        return 1;
    }
    // Inicio de la ejecución e inicio de medidor de reloj
    locale::global(locale("en_US.UTF-8"));
    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
    wstring wide = converter.from_bytes(argv[1]);
    wstring expresion = converter.from_bytes(argv[2]);
    const wchar_t *wide_cstr = wide.c_str();
    auto start = chrono::high_resolution_clock::now();

    try
    {
        postfix = shuntingYard(wide_cstr);
        postfixAugmented = shuntingYard((wstring(wide_cstr) + L"#").c_str());
        TreeNode *tree = constructSyntaxTree(&postfix);
        TreeNode *treeAugmented = constructSyntaxTree(&postfixAugmented);
        wstring alphabet = getAlphabet(&postfix);
        wcout << "\n----------------------------------------\033[1;37m Por algoritmo McNaughton-Yamada-Thompson \033[0m----------------------------------------" << endl;
        Automata *mcythompson = thompson(tree, alphabet);
        printAutomata(mcythompson);
        generateGraph(mcythompson, L"mcythompson");
        wcout << "\n----------------------------------------\033[1;37m Por Construccion de Subconjuntos \033[0m----------------------------------------" << endl;
        Automata *subset = subsetConstruction(mcythompson);
        printAutomata(subset);
        generateGraph(subset, L"bysubsets");
        wcout << "\n----------------------------------------\033[1;37m Por Construccion directa \033[0m----------------------------------------" << endl;
        size_t pos = alphabet.find(L'ε');
        if (pos != std::wstring::npos)
        {
            alphabet.erase(pos, 1);
        }
        Automata *direct = directConstruction(treeAugmented, alphabet);
        completeAFD(direct);
        printAutomata(direct);
        generateGraph(direct, L"bydirect");
        wcout << "\n----------------------------------------\033[1;37m Por Construccion de Subconjuntos (Minificado) \033[0m----------------------------------------" << endl;
        Automata *minifiedSubset = minifyAutomata(subset);
        printAutomata(minifiedSubset);
        generateGraph(minifiedSubset, L"bysubsetsminified");
        wcout << "\n----------------------------------------\033[1;37m Por Construccion directa (Minificado) \033[0m----------------------------------------" << endl;
        Automata *minifiedDirect = minifyAutomata(direct);
        printAutomata(minifiedDirect);
        generateGraph(minifiedDirect, L"bydirectminified");

        wcout << L"\n\033[1;37mSimulacion de AFD por subconjuntos\033[0m" << endl;
        simulateAutomata(subset, expresion);
        wcout << L"\n\033[1;37mSimulacion de AFD por construccion directa\033[0m" << endl;
        simulateAutomata(direct, expresion);
        wcout << L"\n\033[1;37mSimulacion de AFD por subconjuntos (minificado)\033[0m" << endl;
        simulateAutomata(minifiedSubset, expresion);
        wcout << L"\n\033[1;37mSimulacion de AFD por construccion directa (minificado)\033[0m" << endl;
        simulateAutomata(minifiedDirect, expresion);
        wcout << L"\n\033[1;37mSimulacion de AFN por McNaughton-Yamada-Thompson\033[0m" << endl;
        simulateNFA(mcythompson, expresion);
    }
    catch (const exception &e)
    {
        cerr << "\033[1;31m"
             << "ERROR: " << e.what() << "\033[0m" << endl;
    }

    /* wcout << L"A validar: " << expresion << endl; */

    // Fin de la ejecución e impresión de tiempo transcurrido
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
    wstringstream wss;
    wss << elapsed.count();
    wstring elapsed_wstr = wss.str();
    wcout << L"\n\033[1;32mElapsed time: " << elapsed_wstr << L" ms\n\033[0m\n";
    return 0;
}