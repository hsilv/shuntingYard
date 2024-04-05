#include "graphviz.h"
#include "automata.h"
#include <fstream>
#include <string>
#include <locale>
#include <codecvt>
#include <algorithm>

using namespace std;

string escapeSpecialChars(const string &input)
{
    string output = input;
    size_t pos = 0;
    while ((pos = output.find('\\', pos)) != string::npos)
    {
        output.replace(pos, 1, "\\\\");
        pos += 2;
    }
    pos = 0;
    while ((pos = output.find('"', pos)) != string::npos)
    {
        output.replace(pos, 1, "\\\"");
        pos += 2;
    }
    pos = 0;
    while ((pos = output.find('(', pos)) != string::npos)
    {
        output.replace(pos, 1, "\\(");
        pos += 2;
    }
    pos = 0;
    while ((pos = output.find(')', pos)) != string::npos)
    {
        output.replace(pos, 1, "\\)");
        pos += 2;
    }
    return output;
}

void generateGraph(Automata *automata, wstring filename)
{
    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
    string strFilename = converter.to_bytes(filename);
    ofstream out(strFilename + ".dot");

    string dotScript = "digraph finite_state_machine {\n";
    dotScript += "rankdir=LR;\n";
    dotScript += "size=\"800,800\"\n";
    dotScript += "node [shape = plaintext]; qi [label=\"\"];\n";
    if (automata->start->isAcceptable)
    {
        dotScript += "node [shape = doublecircle];\n";
    }
    else
    {
        dotScript += "node [shape = circle];\n";
    }
    wstring startStateNameW(automata->start->name);
    string startStateName = converter.to_bytes(startStateNameW);
    dotScript += "qi -> " + startStateName + ";\n";

    for (const auto &state : automata->states)
    {
        wstring stateNameW(state->name);
        string stateName = converter.to_bytes(stateNameW);

        if (state->isAcceptable)
        {
            wstring returnTypeW(state->returnType);
            string returnType = converter.to_bytes(returnTypeW);
            dotScript += "node [shape = doublecircle, label=\"" + stateName + "\\nreturns: " + returnType + "\"];\n";
        }
        else
        {
            dotScript += "node [shape = circle]; " + stateName + ";\n";
        }
    }

    for (const auto &transition : automata->transitions)
    {
        wstring fromStateNameW(transition->from->name);
        string fromStateName = converter.to_bytes(fromStateNameW);

        wstring toStateNameW(transition->to->name);
        string toStateName = converter.to_bytes(toStateNameW);

        wstring inputW(transition->input);
        string input = converter.to_bytes(inputW);

        // Escape special characters
        input = escapeSpecialChars(input);

        dotScript += fromStateName + " -> " + toStateName + " [ label = \"" + input + "\" ];\n";
    }

    dotScript += "}\n";

    out << dotScript;
    out.close();

    string svgFilename = strFilename.substr(0, strFilename.find_last_of(".")) + ".svg";
    string command = "dot -Tsvg " + strFilename + ".dot" + " -o " + svgFilename;
    system(command.c_str());
}