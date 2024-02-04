#include "shunting.h"
#include <chrono>
#include <iostream>
#include <thread>
#include "automata.h"
#include "tree.h"

Stack<shuntingToken> postfix;

int main(int argc, char *argv[])
{
    // Inicio de la ejecución e inicio de medidor de reloj
    locale::global(locale("en_US.UTF-8"));
    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
    wstring wide = converter.from_bytes(argv[1]);
    const wchar_t *wide_cstr = wide.c_str();
    wcout << wide_cstr << endl;
    auto start = chrono::high_resolution_clock::now();

    try
    {
        postfix = shuntingYard(wide_cstr);
        TreeNode *tree = constructSyntaxTree(&postfix);
        print2D(tree);
    }
    catch (const exception &e)
    {
        cerr << "\033[1;31m"
             << "ERROR: " << e.what() << "\033[0m" << endl;
    }

    // Fin de la ejecución e impresión de tiempo transcurrido
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
    std::wstringstream wss;
    wss << elapsed.count();
    std::wstring elapsed_wstr = wss.str();
    std::wcout << L"Elapsed time: " << elapsed_wstr << L" ms\n";
    return 0;
}