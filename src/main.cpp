#include "shunting.h"
#include <chrono>
#include <iostream>

Stack<shuntingToken> postfix;

int main(int argc, char *argv[])
{
    locale::global(locale("en_US.UTF-8"));

    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
    wstring wide = converter.from_bytes(argv[1]);

    const wchar_t *wide_cstr = wide.c_str();

    wcout << wide_cstr << endl;

    auto start = chrono::high_resolution_clock::now();

    try
    {
        postfix = shuntingYard(wide_cstr);
    }
    catch (const exception &e)
    {
        cerr << "\033[1;31m"
             << "ERROR: " << e.what() << "\033[0m" << endl;
    }

    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Elapsed time: " << elapsed.count() << " ms\n";
    return 0;
}