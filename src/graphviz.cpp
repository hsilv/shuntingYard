#include "graphviz.h"
#include "automata.h"
#include "yapar.h"
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

void replaceAll(string &str, const string &from, const string &to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
}

void generateLR0Graph(LR0Automata *automata, wstring filename)
{
    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
    string strFilename = converter.to_bytes(filename);
    ofstream out(strFilename + ".dot");

    string dotScript = "digraph LR0Automata {\n";
    dotScript += "rankdir=LR;\n";
    dotScript += "node [shape = none];\n";

    for (const LR0AutomataState &state : automata->states)
    {
        string stateName = converter.to_bytes(state.name);

        // Crear un label HTML para el nodo con el nombre del estado y sus producciones
        stringstream label;
        label << "<";
        label << "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">";
        label << "<TR><TD COLSPAN=\"2\"><B>" << stateName << "</B></TD></TR>";

        for (const GrammarProduction &production : state.productions)
        {
            label << "<TR>";
            label << "<TD ALIGN=\"LEFT\">" << converter.to_bytes(production.left.value) << " -&gt; ";
            for (const GrammarToken &token : production.right)
            {
                string tokenValue = converter.to_bytes(token.value);
                replaceAll(tokenValue, ".", "&#46;");
                label << tokenValue << " ";
            }
            label << "</TD>";
            if (!production.type == Core)
            {
                label << "<TD BGCOLOR=\"gray\"></TD>";
            }
            else
            {
                label << "<TD></TD>";
            }
            label << "</TR>";
        }

        label << "</TABLE>";
        label << ">";

        dotScript += stateName + " [label=" + label.str() + "];\n";
    }

    // Crear aristas para las transiciones
    for (const auto &pair : automata->transitions)
    {
        const wstring &fromName = pair.first;
        const map<GrammarToken, wstring> &toStates = pair.second;

        for (const auto &toPair : toStates)
        {
            const GrammarToken &token = toPair.first;
            const wstring &toName = toPair.second;

            // Crear una arista desde fromName hasta toName
            string fromStateName = converter.to_bytes(fromName);
            string toStateName = converter.to_bytes(toName);
            string tokenValue = converter.to_bytes(token.value);
            replaceAll(tokenValue, ".", "&#46;");

            dotScript += fromStateName + " -> " + toStateName + " [ label = \"" + tokenValue + "\" ];\n";
        }
    }

    // Agregar transiciones a estado de aceptación
    for (const wstring &stateName : automata->acceptanceStates)
    {
        string stateNameStr = converter.to_bytes(stateName);
        dotScript += stateNameStr + " -> accept [ label = \"$\" ];\n";
    }

    // Agregar estado de aceptación
    dotScript += "accept [shape = doublecircle, label = \"\", width = 0.5, fixedsize = true];\n";

    dotScript += "}\n";

    out << dotScript;
    out.close();

    string svgFilename = strFilename.substr(0, strFilename.find_last_of(".")) + ".svg";
    string command = "dot -Tsvg " + strFilename + ".dot" + " -o " + svgFilename;
    system(command.c_str());
}