#include <wx/wx.h>
#include <wx/stc/stc.h>
#include <wx/filedlg.h>
#include <wx/textfile.h>
#include <wx/notebook.h>
#include "symboltable.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include "utils.h"
#include "file.h"
#include "shunting.h"
#include "automata.h"
#include "tree.h"
#include "thompson.h"
#include "graphviz.h"
#include "subsets.h"
#include "direct.h"
#include "minification.h"
#include "simulation.h"
#include "yapar.h"

Stack<shuntingToken> postfix;
Stack<shuntingToken> postfixAugmented;
Automata *automatas;
Automata *yapar;

struct Tables
{
    vector<Symbol *> SymbolTable;
    vector<Symbol *> ErrorTable;
};

vector<Automata *> addRegex(vector<Automata *> automatas, wstring regex, wstring returnType)
{
    try
    {
        wcout << L"Regex: " << regex << endl;
        postfix = shuntingYard(regex.c_str());
        wcout << L"Shunting Construido" << endl;
        TreeNode *tree = constructSyntaxTree(&postfix);
        wcout << L"Sintaxis" << endl;
        wstring alphabet = getAlphabet(&postfix);
        wcout << L"Alfabeto" << endl;
        Automata *mcythompson = thompson(tree, alphabet);
        wcout << L"Autómata" << endl;
        mcythompson = addReturnType(mcythompson, returnType);
        wcout << L"Returned" << endl;
        automatas.push_back(mcythompson);
    }
    catch (const std::exception &e)
    {
        // Manejar la excepción aquí
        std::cerr << "Excepción capturada: " << e.what() << '\n';
    }
    return automatas;
}

Automata *configYalexAutomata()
{
    vector<Automata *> automatas;
    automatas = addRegex(automatas, L"let(( )+|\n)", L"Declaration");
    automatas = addRegex(automatas, L"#include(( )+|\n)", L"Include Statement");
    automatas = addRegex(automatas, L"int(( )+|\n)", L"Integer Type");
    automatas = addRegex(automatas, L"float(( )+|\n)", L"Float Type");
    automatas = addRegex(automatas, L" ", L"Space");
    automatas = addRegex(automatas, L"\n", L"Line Break");
    automatas = addRegex(automatas, L"=( )*", L"Assignment Operator");
    automatas = addRegex(automatas, L"\\(", L"Opening Parenthesis");
    automatas = addRegex(automatas, L"\\)", L"Closing Parenthesis");
    automatas = addRegex(automatas, L"\\{", L"Opening Curly Bracket");
    automatas = addRegex(automatas, L"\\}", L"Closing Curly Bracket");
    automatas = addRegex(automatas, L"\\[", L"Opening Square Bracket");
    automatas = addRegex(automatas, L"\\]", L"Closing Square Bracket");
    automatas = addRegex(automatas, L"\\+", L"Plus Sign");
    automatas = addRegex(automatas, L"\\-", L"Minus Sign");
    automatas = addRegex(automatas, L"\\*", L"Asterisk");
    automatas = addRegex(automatas, L"\\?", L"Question Mark");
    automatas = addRegex(automatas, L";", L"Semicolon");
    automatas = addRegex(automatas, L":", L"Colon");
    automatas = addRegex(automatas, L"%", L"Percent");
    automatas = addRegex(automatas, L"\\|", L"Vertical Bar");
    automatas = addRegex(automatas, L"\\\\", L"Backslash");
    automatas = addRegex(automatas, L"/", L"Slash");
    automatas = addRegex(automatas, L"\\^", L"Caret");
    automatas = addRegex(automatas, L"\\$", L"Dollar Sign");
    automatas = addRegex(automatas, L"\\#", L"Sharp");
    automatas = addRegex(automatas, L"\\.", L"Dot");
    automatas = addRegex(automatas, L"\\,", L"Comma");
    automatas = addRegex(automatas, L"return( )+", L"Return Statement");
    automatas = addRegex(automatas, L"rule( )+", L"Rule");
    automatas = addRegex(automatas, L"\"", L"Double Quote");
    automatas = addRegex(automatas, L"\'", L"Single Quote");
    automatas = addRegex(automatas, L"\"([a-z]|[A-Z]|[0-9]|\\\\| |\\*|\\?|\\+|\\.|\\-|\\>|\\<|\\=|:|_|\'|\\(|\\)|/| |;|%|$)+\"", L"String Character");
    automatas = addRegex(automatas, L"\'([a-z]|[A-Z]|[0-9]|\\\\| |\\*|\\?|\\+|\\.|\\-|\\>|\\<|\\=|:|_|\"|\\(|\\)|/| |;|%|$)+\'", L"Regular Character");
    automatas = addRegex(automatas, L"([a-z]|[A-Z])+([a-z]|[A-Z]|[0-9])*", L"Identifier");
    automatas = addRegex(automatas, L"entrypoint", L"Entrypoint");
    automatas = addRegex(automatas, L"eof", L"End of File");
    /* automatas = addRegex(automatas, L"[0-9]*", L"Number"); */
    return joinAutomatas(automatas);
}

Automata *configYaParAutomata()
{
    vector<Automata *> automatas;
    automatas = addRegex(automatas, L"%", L"Percent");
    automatas = addRegex(automatas, L" ", L"Space");
    automatas = addRegex(automatas, L"token", L"Token Statement");
    automatas = addRegex(automatas, L"([A-Z])+(_|[0-9])*", L"Token Identifier");
    automatas = addRegex(automatas, L"([a-z])+(_|[0-9])*", L"Production Identifier");
    automatas = addRegex(automatas, L":", L"Colon");
    automatas = addRegex(automatas, L"\\|", L"Vertical Bar");
    automatas = addRegex(automatas, L";", L"Semicolon");
    automatas = addRegex(automatas, L"\n", L"Line Break");
    return joinAutomatas(automatas);
}

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString &title);
    wxStyledTextCtrl *CreateEditor();
    void OnNew(wxCommandEvent &event);
    void OnOpen(wxCommandEvent &event);
    void OnSave(wxCommandEvent &event);
    void OnKeyDown(wxKeyEvent &event);
    void OnExit(wxCommandEvent &event); // Nuevo método para manejar el evento de "Exit"
    void OnYes(wxCommandEvent &event);

private:
    wxNotebook *notebook;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_NEW, MyFrame::OnNew)
        EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
            EVT_MENU(wxID_SAVE, MyFrame::OnSave)
                EVT_MENU(wxID_EXIT, MyFrame::OnExit)
                    EVT_MENU(wxID_YES, MyFrame::OnYes)
                        wxEND_EVENT_TABLE()

                            wxIMPLEMENT_APP(MyApp);

wxStyledTextCtrl *MyFrame::CreateEditor()
{
    wxStyledTextCtrl *textCtrl = new wxStyledTextCtrl(notebook, wxID_ANY);
    textCtrl->Bind(wxEVT_KEY_DOWN, &MyFrame::OnKeyDown, this); // Cambia a este evento

    // Habilita la numeración de líneas
    textCtrl->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    textCtrl->SetMarginWidth(0, textCtrl->TextWidth(wxSTC_STYLE_LINENUMBER, "_99999"));

    // Habilita las tabulaciones
    textCtrl->SetTabWidth(4);
    textCtrl->SetUseTabs(false);

    return textCtrl;
}

bool MyApp::OnInit()
{
    automatas = configYalexAutomata();
    printAutomata(automatas);
    /* generateGraph(automatas, L"YaLex"); */

    yapar = configYaParAutomata();
    printAutomata(yapar);
    MyFrame *frame = new MyFrame("Code Editor");
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString &title)
    : wxFrame(NULL, wxID_ANY, title)
{
    // Crea un nuevo notebook
    notebook = new wxNotebook(this, wxID_ANY);

    // Crea un nuevo editor de texto
    wxStyledTextCtrl *textCtrl = CreateEditor();

    // Agrega el editor de texto como una nueva pestaña en el notebook
    notebook->AddPage(textCtrl, "New Document", true);

    // Crea un nuevo menú
    wxMenu *fileMenu = new wxMenu;

    // Agrega elementos al menú
    fileMenu->Append(wxID_NEW, "&New");
    fileMenu->Append(wxID_OPEN, "&Open");
    fileMenu->Append(wxID_SAVE, "&Save");
    fileMenu->Append(wxID_EXIT, "E&xit");

    // Crea un nuevo menú para Code
    wxMenu *codeMenu = new wxMenu;

    // Agrega elementos al menú Code
    codeMenu->Append(wxID_YES, "&Build");

    // Crea una nueva barra de menú
    wxMenuBar *menuBar = new wxMenuBar;

    // Agrega el menú a la barra de menú
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(codeMenu, "&Code");

    // Asigna la barra de menú a la ventana
    SetMenuBar(menuBar);

    // Establece el tamaño inicial de la ventana
    SetSize(wxSize(800, 600));

    // Vincula el evento de guardar al método OnSave
    Bind(wxEVT_MENU, &MyFrame::OnSave, this, wxID_SAVE);
}

Tables analyzeLexical(Automata *automata, wstring strText)
{
    vector<Symbol *> SymbolTable = vector<Symbol *>();
    vector<Symbol *> ErrorTable = vector<Symbol *>();
    Tables tables = Tables();

    int fileSize = strText.length();
    int processedSize = 0;

    wcout << strText << endl;

    wstring::iterator begin = strText.begin();
    wstring::iterator end = strText.end();
    wstring::iterator current = strText.begin();
    wstring::iterator latestAccepted = strText.begin();
    wstring::iterator latestRejected = strText.begin();
    int numberLine = 1;
    int numberCol = 1;

    wstring latestAcceptedSymbol = L"";

    // Define los delimitadores
    wstring delimiters = L"\t\n ";

    vector<wstring> tokens;
    size_t startString = 0;
    size_t endString = strText.find_first_of(delimiters);

    while (endString != wstring::npos)
    {
        // Añade el token, incluyendo el delimitador
        tokens.push_back(strText.substr(startString, endString - startString + 1));

        // Encuentra el inicio del siguiente token
        startString = endString + 1;
        endString = strText.find_first_of(delimiters, startString);
    }

    // Añade el último token si no termina con un delimitador
    if (startString != strText.length())
    {
        tokens.push_back(strText.substr(startString));
    }

    for (const wstring &token : tokens)
    {
        /* wcout << token << L" " << token.length() << endl; */
        processedSize += token.length();
        wcout << L"Processed: " << processedSize << L" of " << fileSize << endl;
        strText = token;
        while (strText.length() > 0)
        {
            while (current != strText.end())
            {
                wstring substring = strText.substr(0, distance(begin, current + 1));
                wstring result = simulateNFA(automata, substring);

                if (result != L"")
                {
                    if (*current == L'\n')
                    {
                        numberLine++;
                        numberCol = 1;
                    }
                    else
                    {
                        numberCol = substring.length() - substring.rfind(L'\n');
                    }

                    latestAccepted = current;
                    latestAcceptedSymbol = result;
                    latestRejected = begin;
                }
                else
                {
                    latestRejected = current;
                }
                current++;
            }

            if (latestAcceptedSymbol != L"" || latestAccepted != begin)
            {
                wstring acceptedSubstring = strText.substr(0, distance(begin, latestAccepted + 1));
                Symbol *symbol = new Symbol();
                symbol->value = wcsdup(acceptedSubstring.c_str());
                symbol->type = wcsdup(latestAcceptedSymbol.c_str());
                symbol->numberLine = numberLine;
                symbol->numberColumn = numberCol;
                SymbolTable.push_back(symbol);
                strText.erase(0, distance(begin, latestAccepted + 1));
                begin = strText.begin();
                current = strText.begin();
                latestAccepted = strText.begin();
                latestRejected = strText.begin();
                end = strText.end();
                latestAcceptedSymbol = L"";
            }
            else
            {
                latestAcceptedSymbol = L"";
                wstring rejectedSubstring = strText.substr(0, distance(begin, latestRejected + 1));

                wstring::iterator beginRejected = begin;
                wstring::iterator endRejected = latestRejected;
                wstring::iterator evalCursor = beginRejected;
                wstring::iterator delimiter;

                wstring refusedToken = L"";

                while (evalCursor != endRejected)
                {
                    int bias = distance(evalCursor, endRejected) + 1;
                    int startSubs = distance(beginRejected, evalCursor);

                    for (int j = 1; j <= bias; j++)
                    {
                        wstring substringEval = strText.substr(startSubs, j);
                        wstring resultSim = simulateNFA(automata, substringEval);

                        if (resultSim != L"")
                        {
                            refusedToken = strText.substr(0, startSubs);
                            delimiter = evalCursor;
                            break;
                        }
                    }
                    if (delimiter == evalCursor)
                    {
                        break;
                    }
                    evalCursor++;
                }

                if (evalCursor == endRejected)
                {
                    wstring substringEval = strText.substr(distance(begin, evalCursor), 1);
                    wstring resultSim = simulateNFA(automata, substringEval);

                    if (resultSim != L"")
                    {
                        refusedToken = strText.substr(0, distance(beginRejected, evalCursor));
                        delimiter = evalCursor;
                    }
                }

                Symbol *symbol = new Symbol();
                if (refusedToken == L"")
                {
                    symbol->value = wcsdup(rejectedSubstring.c_str());
                    symbol->type = L"Error";
                    symbol->numberLine = numberLine;
                    symbol->numberColumn = numberCol;
                    ErrorTable.push_back(symbol);
                    strText.erase(0, distance(begin, latestRejected + 1));
                    begin = strText.begin();
                    current = strText.begin();
                    latestAccepted = strText.begin();
                    latestRejected = strText.begin();
                    end = strText.end();
                }
                else
                {
                    symbol->value = wcsdup(refusedToken.c_str());
                    symbol->type = L"Error";
                    symbol->numberLine = numberLine;
                    symbol->numberColumn = numberCol;
                    ErrorTable.push_back(symbol);
                    strText.erase(0, distance(begin, delimiter));
                    begin = strText.begin();
                    current = strText.begin();
                    latestAccepted = strText.begin();
                    latestRejected = strText.begin();
                    end = strText.end();
                }
            }

            if (current == strText.end())
            {
                break;
            }
        }
    }

    tables.SymbolTable = SymbolTable;
    tables.ErrorTable = ErrorTable;

    return tables;
}
#include <cstdlib>
#include <wx/filedlg.h>

void MyFrame::OnYes(wxCommandEvent &event)
{
    vector<Symbol *> SymbolTable = vector<Symbol *>();
    vector<Symbol *> ErrorTable = vector<Symbol *>();
    wxStyledTextCtrl *textCtrl = (wxStyledTextCtrl *)notebook->GetCurrentPage();
    wxString text = textCtrl->GetText();
    wstring strText(text.wc_str());

    wstring cleanedText = cleanText(strText);
    strText = cleanedText;

    Tables tables = analyzeLexical(automatas, strText);

    SymbolTable = tables.SymbolTable;
    ErrorTable = tables.ErrorTable;

    wcout << endl;
    wcout << "Symbol Table" << endl;

    for (Symbol *symbol : SymbolTable)
    {
        wcout << L"Token: " << symbol->value << L" Type: " << symbol->type << L" Line: " << symbol->numberLine << L" Column: " << symbol->numberColumn << endl;
    }

    int currentPageIndex = notebook->GetSelection();
    int nextPageIndex = currentPageIndex + 1;

    wstring nextText;

    // Verifica si la página siguiente existe
    if (nextPageIndex < notebook->GetPageCount())
    {
        wxStyledTextCtrl *nextTextCtrl = (wxStyledTextCtrl *)notebook->GetPage(nextPageIndex);
        wxString pageText = nextTextCtrl->GetText();
        nextText = wstring(pageText.wc_str());
    }
    else
    {
        // Lanzar una excepción que diga que hace falta que abras una página en yapar
        wcout << "YaPar page not found" << endl;
        throw std::runtime_error("YaPar page not found");
    }

    std::vector<Symbol *>::iterator current = SymbolTable.begin();
    std::vector<Symbol *>::iterator eof = SymbolTable.end();

    std::map<std::wstring, std::vector<Symbol *>> variables;
    std::map<wstring, wstring> rules;

    bool waitingForIdentifier = false;
    bool waitingForEqual = false;
    bool collectingValue = false;
    bool inRule = false;
    bool allowAddRule = false;
    bool waitingForRuleIdentifier = false;
    std::wstring currentIdentifier;
    std::vector<Symbol *> currentValue;
    std::wstring ruleRegex;

    while (current != eof)
    {
        Symbol *symbol = *current;

        if (wcscmp(symbol->type, L"Declaration") == 0 || (wcscmp(symbol->type, L"Rule") == 0 && !inRule))
        {
            inRule = wcscmp(symbol->type, L"Rule") == 0;
            waitingForIdentifier = true;
        }
        else if (waitingForIdentifier && wcscmp(symbol->type, L"Identifier") == 0)
        {
            waitingForIdentifier = false;
            waitingForEqual = true;
            currentIdentifier = symbol->value;
        }
        else if (waitingForEqual && wcscmp(symbol->type, L"Assignment Operator") == 0)
        {
            waitingForEqual = false;
            collectingValue = true;
        }
        else if (!inRule && collectingValue)
        {
            if (wcscmp(symbol->type, L"Identifier") == 0)
            {
                // If the identifier is found in the variables map, replace it with its value
                if (variables.find(symbol->value) != variables.end())
                {
                    std::vector<Symbol *> value = variables[symbol->value];
                    currentValue.insert(currentValue.end(), value.begin(), value.end());
                }
                else
                {
                    currentValue.push_back(symbol);
                }
            }
            else if (wcscmp(symbol->type, L"Regular Character") == 0)
            {
                // Ignore the first and last character of the value
                Symbol *newSymbol = new Symbol;
                newSymbol->type = symbol->type;
                size_t len = wcslen(symbol->value);
                newSymbol->value = new wchar_t[len - 1];
                wcsncpy(newSymbol->value, symbol->value + 1, len - 2);
                newSymbol->value[len - 2] = '\0';
                newSymbol->numberLine = symbol->numberLine;
                newSymbol->numberColumn = symbol->numberColumn;
                currentValue.push_back(newSymbol);
            }
            else if (wcscmp(symbol->type, L"Line Break") == 0)
            {
                collectingValue = false;
                variables[currentIdentifier] = currentValue;
                currentValue.clear();
            }
            else
            {
                currentValue.push_back(symbol);
            }
        }
        else if (inRule)
        {
            if (wcscmp(symbol->type, L"Space") == 0 || wcscmp(symbol->type, L"Vertical Bar") == 0 || wcscmp(symbol->type, L"Line Break") == 0)
            {
            }
            else if (!allowAddRule && wcscmp(symbol->type, L"Identifier") == 0)
            {
                if (variables.find(symbol->value) != variables.end())
                {
                    std::vector<Symbol *> value = variables[symbol->value];
                    std::wstring valueStr;
                    for (auto &val : value)
                    {
                        valueStr += val->value;
                    }
                    wcout << L"Value: " << valueStr << endl;
                    ruleRegex += valueStr;
                }
                else
                {
                    wcout << L"Error: Identifier " << symbol->value << L" not found" << endl;
                }
            }
            else if (allowAddRule == false && (wcscmp(symbol->type, L"Regular Character") == 0 || wcscmp(symbol->type, L"String Character") == 0))
            {
                std::wstring valueStr(symbol->value);
                if (valueStr.length() > 2) // Check if the string has more than 2 characters
                {
                    // Add the value of the symbol to ruleRegex, excluding the first and last character
                    ruleRegex += valueStr.substr(1, valueStr.length() - 2);
                }
            }
            else if (wcscmp(symbol->type, L"Opening Curly Bracket") == 0)
            {
                wcout << L"Rule Opening Curly Bracket" << endl;
                allowAddRule = true;
            }

            else if (allowAddRule && wcscmp(symbol->type, L"Return Statement") == 0)
            {
                wcout << L"Waiting for Identifier" << endl;
                waitingForRuleIdentifier = true;
            }
            else if (allowAddRule && waitingForRuleIdentifier /*  && wcscmp(symbol->type, L"Identifier") == 0 */)
            {
                // Add the rule to the rules map, the identifier will be key, and the ruleRegex will be the value
                wcout << L"Adding Rule: " << symbol->value << endl;
                rules[symbol->value] = ruleRegex;
                ruleRegex = L"";
                waitingForRuleIdentifier = false;
            }
            else if (allowAddRule && wcscmp(symbol->type, L"Closing Curly Bracket") == 0)
            {
                allowAddRule = false;
            }
            else if (allowAddRule == false)
            {
                std::wstring valueStr(symbol->value);
                ruleRegex += valueStr;
            }
            else
            {
                wcout << L"Error: Unexpected symbol " << symbol->value << L" in rule" << endl;
            }
        }

        current++;
    }

    wcout << endl;
    wcout << L"Variables: " << endl;
    for (auto const &variable : variables)
    {
        wcout << variable.first << L": ";
        for (Symbol *symbol : variable.second)
        {
            wcout << symbol->value;
        }
        wcout << endl;
    }

    wcout << endl;
    wcout << L"Rules: " << endl;
    for (auto const &rule : rules)
    {
        wcout << rule.first << L": " << rule.second << endl;
    }

    vector<Automata *> lexAnalyzer = vector<Automata *>();
    for (auto const &rule : rules)
    {
        if (&rule == nullptr)
        {
            continue;
        }
        wstring regex = rule.second;
        wstring returnType = rule.first;
        lexAnalyzer = addRegex(lexAnalyzer, regex, returnType);
    }

    Automata *mcythompson = joinAutomatas(lexAnalyzer);

    printAutomata(mcythompson);
    generateGraph(mcythompson, L"LexicalAnalyzer");

    wcout
        << "Error Table" << endl;
    for (Symbol *symbol : ErrorTable)
    {
        wcout << L"Token: " << symbol->value << L" Type: " << symbol->type << L" Line: " << symbol->numberLine << L" Column: " << symbol->numberColumn << endl;
    }

    nextText = cleanYaPar(nextText);
    wcout << nextText << endl;

    Tables yaparTables = analyzeLexical(yapar, nextText);

    /*     for (Symbol *symbol : yaparTables.SymbolTable)
        {
            wcout << L"Token: " << symbol->value << L" Type: " << symbol->type << L" Line: " << symbol->numberLine << L" Column: " << symbol->numberColumn << endl;
        } */

    std::vector<Symbol *>::iterator yaparCurrent = yaparTables.SymbolTable.begin();
    std::vector<Symbol *>::iterator yaparEof = yaparTables.SymbolTable.end();
    bool tokenSection = true;
    bool waitingTokenStatement = false;
    bool waitingColon = false;
    bool inProduction = false;
    Grammar grammar;
    GrammarToken start;
    GrammarToken productionHeader;
    vector<GrammarToken> productionBody;

    map<wstring, wstring> yaparTerminals;

    set<GrammarToken> terminals;
    set<GrammarToken> nonTerminals;

    while (yaparCurrent != yaparEof)
    {
        Symbol *symbol = *yaparCurrent;
        /* wcout << L"Token: " << symbol->value << L" Type: " << symbol->type << L" Line: " << symbol->numberLine << L" Column: " << symbol->numberColumn << endl; */
        if (wcscmp(symbol->type, L"Percent") == 0)
        {
            if (waitingTokenStatement)
            {
                tokenSection = false;
            }
            else
            {
                waitingTokenStatement = true;
            }
            /* wcout << L"Token Section: " << tokenSection << endl; */
        }
        else if (tokenSection && waitingTokenStatement && wcscmp(symbol->type, L"Token Statement") == 0)
        {
            waitingTokenStatement = false;
        }
        else if (tokenSection && !waitingTokenStatement && wcscmp(symbol->type, L"Token Identifier") == 0)
        {
            bool found = false;
            std::wstring value(symbol->value);
            std::wstring matchedRuleSecond;
            for (auto const &rule : rules)
            {
                if (rule.first.find(value) != std::wstring::npos) // Usa la variable type
                {
                    found = true;
                    matchedRuleSecond = rule.second;
                    break;
                }
            }
            if (!found)
            {
                wcout << L"Exception, Token Identifier NOT FOUND: " << value << endl;
                throw std::runtime_error("Token identifier not found in rules");
            }
            else
            {
                yaparTerminals[value] = matchedRuleSecond;
            }
            /* wcout << L"Token Identifier: " << symbol->value << endl; */
        }
        else if (!tokenSection && !inProduction && wcscmp(symbol->type, L"Production Identifier") == 0)
        {
            waitingColon = true;
            productionHeader.value = symbol->value;
            productionHeader.type = NonTerminal;

            if (start.value == L"")
            {
                start = productionHeader;
            }
            /* wcout << L"Insertando no terminal: " << symbol->value << L" " << nonTerminals.size() << endl; */
            nonTerminals.insert(productionHeader);
        }
        else if (waitingColon && wcscmp(symbol->type, L"Colon") == 0)
        {
            waitingColon = false;
            inProduction = true;
        }
        else if (!tokenSection && inProduction && wcscmp(symbol->type, L"Production Identifier") == 0)
        {
            GrammarToken token;
            token.value = symbol->value;
            token.type = NonTerminal;
            /* if (nonTerminals.find(token) == nonTerminals.end())
            {
                wcout << L"Exception, Production Identifier NOT FOUND in nonTerminals: " << token.value << endl;
                throw std::runtime_error("Production identifier not found in nonTerminals");
            } */
            productionBody.push_back(token);
            nonTerminals.insert(token);
            /*  wcout << L"Insertando no terminal: " << symbol->value << L" " << nonTerminals.size() << endl; */
        }
        else if (!tokenSection && inProduction && wcscmp(symbol->type, L"Token Identifier") == 0)
        {
            wstring value(symbol->value);
            if (yaparTerminals.find(value) == yaparTerminals.end())
            {
                wcout << L"Exception, Token Identifier NOT FOUND in YaLex: " << value << endl;
                throw std::runtime_error("Token identifier not found in YaLex");
            }
            value = yaparTerminals[value];
            GrammarToken token;
            token.value = value;
            token.type = Terminal;
            productionBody.push_back(token);
            terminals.insert(token);
        }
        else if (!tokenSection && inProduction && (wcscmp(symbol->type, L"Vertical Bar") == 0 || wcscmp(symbol->type, L"Semicolon") == 0))
        {
            // Imprimir el cuerpo de la producción
            /* wcout << L"Production Body: ";
            for (auto const &token : productionBody)
            {
                wcout << token.value << L" ";
            } */

            GrammarProduction production;
            production.left = productionHeader;
            production.right = productionBody;
            production.type = Core;
            grammar.productions.push_back(production);
            productionBody.clear();

            if (wcscmp(symbol->type, L"Semicolon") == 0)
            {
                inProduction = false;
            }
        }
        else if (!tokenSection && inProduction && wcscmp(symbol->type, L"Colon") == 0)
        {
            wcout << L"YaPar Colon Syntax Error" << endl;
            throw std::runtime_error("YaPar Colon Syntax Error");
        }
        yaparCurrent++;
    }

    /* for (auto const &terminal : yaparTerminals)
    {
        wcout << terminal.first << L": " << terminal.second << endl;
    } */
    grammar.start = start;
    grammar.nonTerminals = nonTerminals;
    grammar.terminals = terminals;
    grammar = augment(grammar);
    /*  wcout << L"\nProducciones: " << endl;
     for (auto const &production : grammar.productions)
     {
         wcout << production.left.value << L" -> ";
         for (auto const &token : production.right)
         {
             wcout << token.value << L" ";
         }
         wcout << endl;
     }

     wcout << L"\nSímbolo Inicial: " << grammar.start.value << endl;
     wcout << L"\nTerminales: " << endl;
     for (auto const &terminal : grammar.terminals)
     {
         wcout << terminal.value << endl;
     }

     wcout << endl;

     wcout << L"No Terminales: " << endl;
     for (auto const &nonTerminal : grammar.nonTerminals)
     {
         wcout << nonTerminal.value << endl;
     }

     set<GrammarProduction> toClosure;
     toClosure.insert(grammar.productions.at(0));

     set<GrammarProduction> closureSet = closure(grammar, toClosure);

     for (auto const &production : closureSet)
     {
         wcout << production.left.value << L" -> ";
         for (auto const &token : production.right)
         {
             wcout << token.value << L" ";
         }
         wcout << endl;
     } */

    /* for (const GrammarToken terminal : grammar.nonTerminals)
    {
        wcout << L"Terminal: " << terminal.value << endl;
        set<GrammarProduction> gotoset = gotoSet(grammar, closureSet, terminal);
        wcout << L"\nGoto Set: " << gotoset.size() << endl;
        for (auto const &production : gotoset)
        {
            wcout << production.left.value << L" -> ";
            for (auto const &token : production.right)
            {
                wcout << token.value << L" ";
            }
            wcout << endl;
        }
    }

    for (const GrammarToken terminal : grammar.terminals)
    {
        wcout << L"Terminal: " << terminal.value << endl;
        set<GrammarProduction> gotoset = gotoSet(grammar, closureSet, terminal);
        wcout << L"\nGoto Set: " << gotoset.size() << endl;
        for (auto const &production : gotoset)
        {
            wcout << production.left.value << L" -> ";
            for (auto const &token : production.right)
            {
                wcout << token.value << L" ";
            }
            wcout << endl;
        }
    } */
    LR0Automata lr0automata = build(grammar);
    generateLR0Graph(&lr0automata, L"LR0Automata");
}

void MyFrame::OnExit(wxCommandEvent &event)
{
    if (notebook->GetPageCount() > 1)
    {
        notebook->DeletePage(notebook->GetSelection());
    }
    else if (notebook->GetPageCount() == 1)
    {
        notebook->DeletePage(notebook->GetSelection());
        Close(true);
    }
    else
    {
        Close(true);
    }
}

void MyFrame::OnSave(wxCommandEvent &event)
{
    wxStyledTextCtrl *textCtrl = (wxStyledTextCtrl *)notebook->GetCurrentPage();
    wxString filename = notebook->GetPageText(notebook->GetSelection());

    if (filename == "New Document")
    {
        wxFileDialog saveFileDialog(this, _("Save file"), "", "",
                                    "All files (*.*)|*.*", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

        if (saveFileDialog.ShowModal() == wxID_CANCEL)
            return;

        filename = saveFileDialog.GetPath();
        notebook->SetPageText(notebook->GetSelection(), saveFileDialog.GetFilename());
    }

    wxTextFile file(filename);
    if (file.Exists())
        file.Clear();

    file.Create();
    wxString text = textCtrl->GetText();
    file.AddLine(text);
    file.Write();
    file.Close();
}

void MyFrame::OnNew(wxCommandEvent &event)
{
    wxStyledTextCtrl *textCtrl = CreateEditor();
    notebook->AddPage(textCtrl, "New Document", true);
}

void MyFrame::OnOpen(wxCommandEvent &event)
{
    wxFileDialog openFileDialog(this, _("Open file"), "", "",
                                "All files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxStyledTextCtrl *textCtrl = (wxStyledTextCtrl *)notebook->GetCurrentPage();
    if (textCtrl->GetText().IsEmpty())
    {
        textCtrl->ClearAll();
        notebook->SetPageText(notebook->GetSelection(), openFileDialog.GetFilename());
    }
    else
    {
        textCtrl = CreateEditor();
        notebook->AddPage(textCtrl, openFileDialog.GetFilename(), true);
    }

    wxTextFile file;
    file.Open(openFileDialog.GetPath());
    textCtrl->SetText(file.GetFirstLine());

    while (!file.Eof())
    {
        textCtrl->AppendText("\n");
        textCtrl->AppendText(file.GetNextLine());
    }
}

void MyFrame::OnKeyDown(wxKeyEvent &event)
{
    int code = event.GetRawKeyCode();
    bool shiftDown = event.ShiftDown();

    char ch = 0;
    char close_ch = 0;

    if (code == 123 || code == 91)
    {
        if (shiftDown)
        {
            ch = '[';
            close_ch = ']';
        }
        else
        {
            ch = '{';
            close_ch = '}';
        }
    }
    else if (code == 125 || code == 93)
    {
        if (shiftDown)
        {
            ch = ']';
            close_ch = '[';
        }
        else
        {
            ch = '}';
            close_ch = '{';
        }
    }
    else if (code == 40 || code == 41)
    {
        if (shiftDown)
        {
            ch = '(';
            close_ch = ')';
        }
        else
        {
            ch = ')';
            close_ch = '(';
        }
    }

    if (ch != 0 && close_ch != 0)
    {
        wxStyledTextCtrl *textCtrl = (wxStyledTextCtrl *)notebook->GetCurrentPage();
        long selStart = textCtrl->GetSelectionStart();
        long selEnd = textCtrl->GetSelectionEnd();

        if (selStart != selEnd)
        {
            wxString selectedText = textCtrl->GetSelectedText();
            wxString newText = ch + selectedText + close_ch;
            textCtrl->SetSelection(selStart, selEnd);
            textCtrl->ReplaceSelection(newText);
            event.Skip(false);
        }
        else
        {
            wxString newText = wxString(ch) + close_ch;
            textCtrl->AddText(newText);
            textCtrl->GotoPos(textCtrl->GetCurrentPos() - 1);
            event.Skip(false);
        }
    }
    else
    {
        event.Skip();
    }
}