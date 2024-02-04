#include "shunting.h"
#include <chrono>
#include <iostream>

int main(int argc, char *argv[])
{
    auto start = std::chrono::high_resolution_clock::now();
    shuntingYard(argv[1]);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Elapsed time: " << elapsed.count() << " ms\n";
    return 0;
}