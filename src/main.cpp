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

Stack<shuntingToken> postfix;

int main(int argc, char *argv[])
{
    // Inicio de la ejecución e inicio de medidor de reloj
    locale::global(locale("en_US.UTF-8"));
    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
    wstring wide = converter.from_bytes(argv[1]);
    const wchar_t *wide_cstr = wide.c_str();
    auto start = chrono::high_resolution_clock::now();

    try
    {
        postfix = shuntingYard(wide_cstr);
        TreeNode *tree = constructSyntaxTree(&postfix);
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
        directConstruction(tree, alphabet);
        /* Automata *direct = directConstruction(tree, alphabet);
        printAutomata(direct);
        generateGraph(direct, L"bydirect"); */
    }
    catch (const exception &e)
    {
        cerr << "\033[1;31m"
             << "ERROR: " << e.what() << "\033[0m" << endl;
    }

    // Fin de la ejecución e impresión de tiempo transcurrido
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
    wstringstream wss;
    wss << elapsed.count();
    wstring elapsed_wstr = wss.str();
    wcout << L"\n\033[1;32mElapsed time: " << elapsed_wstr << L" ms\n\033[0m\n";
    return 0;
}