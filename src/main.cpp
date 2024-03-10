#include <wx/wx.h>
#include <wx/stc/stc.h>
#include <wx/filedlg.h>
#include <wx/textfile.h>
#include <wx/notebook.h>
#include "symboltable.h"
#include "shunting.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include "automata.h"
#include "tree.h"
#include "thompson.h"
#include "graphviz.h"
#include "subsets.h"
#include "direct.h"
#include "minification.h"
#include "simulation.h"
#include <algorithm>
#include <cctype>

Stack<shuntingToken> postfix;
Stack<shuntingToken> postfixAugmented;

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

bool evalRegex(wstring regex, wstring input)
{
    postfix = shuntingYard(regex.c_str());
    /* postfixAugmented = shuntingYard((regex + L"#").c_str()); */
    TreeNode *tree = constructSyntaxTree(&postfix);
    /* TreeNode *treeAugmented = constructSyntaxTree(&postfixAugmented); */
    /* TreeNode *augmentedParsedTree = parseTree(treeAugmented); */
    wstring alphabet = getAlphabet(&postfix);
    Automata *mcythompson = thompson(tree, alphabet);
    Automata *subset = subsetConstruction(mcythompson);
    Automata *subsetCopy = deepCopyAutomata(subset);
    Automata *minifiedSubset = minifyAutomata(subsetCopy);
    return simulateNFA(mcythompson, input);
}

bool MyApp::OnInit()
{
    /* if (argc != 3)
    {
        cerr << "\033[1;31m"
             << "ERROR: Debe ingresar una expresión regular y una cadena a validar"
             << "\033[0m" << endl;
        return 1;
    } */
    // Inicio de la ejecución e inicio de medidor de reloj
    /* locale::global(locale("en_US.UTF-8"));
    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
    wstring wide = converter.from_bytes(argv[1]);
    wstring expresion = converter.from_bytes(argv[2]);
    const wchar_t *wide_cstr = wide.c_str(); */
    auto start = chrono::high_resolution_clock::now();

    /*  try
     {
         postfix = shuntingYard(wide_cstr);
         postfixAugmented = shuntingYard((wstring(wide_cstr) + L"#").c_str());
         TreeNode *tree = constructSyntaxTree(&postfix);
         print2D(tree);
         TreeNode *treeAugmented = constructSyntaxTree(&postfixAugmented);
         TreeNode *augmentedParsedTree = parseTree(treeAugmented);
         print2D(treeAugmented);
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
         Automata *direct = directConstruction(augmentedParsedTree, alphabet);
         completeAFD(direct);
         printAutomata(direct);
         generateGraph(direct, L"bydirect");
         wcout << "\n----------------------------------------\033[1;37m Por Construccion de Subconjuntos (Minificado) \033[0m----------------------------------------" << endl;

         Automata *subsetCopy = deepCopyAutomata(subset);
         printAutomata(subsetCopy);
         Automata *minifiedSubset = minifyAutomata(subsetCopy);
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
     } */

    // Fin de la ejecución e impresión de tiempo transcurrido
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
    wstringstream wss;
    wss << elapsed.count();
    wstring elapsed_wstr = wss.str();
    wcout << L"\n\033[1;32mElapsed time: " << elapsed_wstr << L" ms\n\033[0m\n";
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
    codeMenu->Append(wxID_YES, "&Build Lexical Analyzer");

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

void MyFrame::OnYes(wxCommandEvent &event)
{
    // Obtiene el control de texto de la página actual
    wxStyledTextCtrl *textCtrl = (wxStyledTextCtrl *)notebook->GetCurrentPage();

    // Obtiene todo el texto del control de texto
    wxString text = textCtrl->GetText();

    // Convierte wxString a std::wstring
    std::wstring strText(text.wc_str());

    std::wstring cleanedText;
    bool inComment = false;

    for (auto it = strText.begin(); it != strText.end(); ++it)
    {
        if (!inComment && *it == L'(' && *(it + 1) == L'*')
        {
            inComment = true;
            ++it;
        }
        else if (inComment && *it == L'*' && *(it + 1) == L')')
        {
            inComment = false;
            ++it;
        }
        else if (!inComment)
        {
            cleanedText += *it;
        }
    }

    strText = cleanedText;

    // Reemplaza los saltos de línea por un espacio
    /* std::replace(strText.begin(), strText.end(), L'\n', L' '); */

    struct Patterns
    {
        std::wstring regex;
        std::wstring type;
        Automata *automata;
    };

    std::vector<Symbol> errores;
    std::vector<Symbol> lexemasAceptados;
    std::vector<Patterns> regexes; // Tus expresiones regulares

    // Añade tus expresiones regulares a regexes
    /* regexes.push_back({L"([a-z]|[A-Z]|[0-9])+-([a-z]|[A-Z]|[0-9])", L"Range"}); */
    regexes.push_back({L"\\(", L"Opening parenthesis"});
    regexes.push_back({L"\\)", L"Closing parenthesis"});
    regexes.push_back({L"\\{", L"Opening curly bracket"});
    regexes.push_back({L"\\}", L"Closing curly bracket"});
    regexes.push_back({L"\\[", L"Opening square bracket"});
    regexes.push_back({L"\\]", L"Closing square bracket"});
    regexes.push_back({L"\\+", L"Plus sign"});
    regexes.push_back({L"\\-", L"Minus sign"});
    regexes.push_back({L"\\*", L"Asterisk"});
    regexes.push_back({L"\\?", L"Question mark"});
    regexes.push_back({L"\\;", L"Semicolon"});
    regexes.push_back({L"\\|", L"Vertical bar"});
    regexes.push_back({L"\\\\", L"Backslash"});
    regexes.push_back({L"/", L"Slash"});
    regexes.push_back({L"\\^", L"Caret"});
    regexes.push_back({L"\\$", L"Dollar sign"});
    regexes.push_back({L"#include", L"Include Statement"});
    regexes.push_back({L"\\#", L"Sharp"});
    regexes.push_back({L"\\.", L"Dot"});
    regexes.push_back({L"let", L"Declaration"});
    regexes.push_back({L"eof", L"End of file"});
    regexes.push_back({L"return", L"Return Statement"});
    regexes.push_back({L"=", L"Assignment"});
    regexes.push_back({L"rule", L"Rule"});
    regexes.push_back({L",", L"Comma"});
    regexes.push_back({L"\"([a-z]|[A-Z]|[0-9]|\\\\|_|\\*|\\?|\\+|\\.|\\-|\\>|\\<|\\=)+\"", L"String Character"});
    regexes.push_back({L"\'([a-z]|[A-Z]|[0-9]|\\\\|_|\\*|\\?|\\+|\\.|\\-|\\>|\\<|\\=)+\'", L"Regular Character"});
    regexes.push_back({L"\"", L"Double quote"});
    regexes.push_back({L"\'", L"Quote"});
    regexes.push_back({L"entrypoint", L"Entrypoint"});
    regexes.push_back({L"[0-9]+", L"Integer"});
    regexes.push_back({L"[0-9]+(\\.[0-9]+)?", L"Decimal"});
    regexes.push_back({L"([a-z]|[A-Z])+([a-z]|[A-Z]|[0-9])*", L"Identifier"});
    regexes.push_back({L" ", L"Space"});
    /* regexes.push_back({L"\\[([a-z]|[A-Z]|[0-9]|\\\\)+-([a-z]|[A-Z]|[0-9]|\\\\)+\\]", L"Character set"}); */

    try
    {
        for (auto &pattern : regexes)
        {
            Stack<shuntingToken> postfixRegex = shuntingYard(pattern.regex.c_str());
            std::wstring alphabet = getAlphabet(&postfixRegex);
            pattern.automata = thompson(constructSyntaxTree(&postfixRegex), alphabet);
            /* std::wstring type = pattern.type;


            std::transform(type.begin(), type.end(), type.begin(),
                           [](wchar_t c)
                           { return std::towlower(c); });


            std::replace(type.begin(), type.end(), L' ', L'_');
            generateGraph(pattern.automata, type); */
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Caught exception: " << e.what() << '\n';
    }
    catch (...)
    {
        std::cerr << "Caught unknown exception\n";
    }

    wcout << "Processing Lexical Analyzer" << endl;
    try
    {
        std::wstring::iterator it = strText.begin();
        int line = 1;
        int column = 1;
        bool inString = false;
        bool inChar = false;
        while (it != strText.end())
        {
            while (it != strText.end() && std::iswspace(*it))
            {
                if (*it == L'\n')
                {
                    ++line;
                    column = 1;
                }
                else
                {
                    ++column;
                }
                ++it;
            }

            if (it == strText.end())
                break;

            std::wstring lexema;
            bool match = false;

            wcout << "Processing: " << *it << endl;

            std::wstring delimiters = L"| #-+*?/^$.;:()[]{},.\n";

            if (inString || inChar || delimiters.find(*it) == std::wstring::npos) // If the character is not a delimiter or we're in a string/char
            {
                lexema += *it;
                if (*it == L'\"')
                {
                    inString = !inString;
                }
                else if (*it == L'\'')
                {
                    inChar = !inChar;
                }
                while (it + 1 != strText.end() && (delimiters.find(*(it + 1)) == std::wstring::npos || inString || inChar))
                {
                    lexema += *++it;
                    ++column;
                    if (*it == L'\"')
                    {
                        inString = !inString;
                    }
                    else if (*it == L'\'')
                    {
                        inChar = !inChar;
                    }
                }
            }
            else if (*it != L' ') // If the character is a delimiter (but not a space)
            {
                lexema += *it;
            }

            for (auto &pattern : regexes)
            {
                /* wcout << "lexema: " << lexema << " pattern: " << pattern.regex << endl; */
                if (simulateNFA(pattern.automata, lexema))
                {
                    match = true;

                    // Create a new Symbol and add it to lexemasAceptados
                    /* wcout << "Matched: " << lexema << L" with " << pattern.type << endl; */
                    Symbol symbol;
                    symbol.type = wcsdup(pattern.type.c_str());
                    symbol.value = wcsdup(lexema.c_str());
                    symbol.numberLine = line;
                    symbol.numberColumn = column - lexema.length() + 1;
                    lexemasAceptados.push_back(symbol);
                    break;
                }
            }

            if (!match)
            {
                // Split lexema into words and add them to errores
                std::wistringstream iss(lexema);
                std::wstring word;
                while (iss >> word)
                {
                    Symbol symbol;
                    symbol.type = L"Error";
                    symbol.value = wcsdup(word.c_str());
                    symbol.numberLine = line;
                    symbol.numberColumn = column - word.length() + 1;
                    errores.push_back(symbol);
                }
            }

            ++it;
            ++column;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Caught unknown exception" << std::endl;
    }
    wcout << endl;

    wcout << L"\n\033[1;37mLexemas aceptados\033[0m" << endl;
    for (auto &lexema : lexemasAceptados)
    {
        if (lexema.type != L"String Character" || lexema.type == L"Regular Character")
        {
            std::wstring value = lexema.value;
            std::replace(value.begin(), value.end(), L'_', L' ');
            delete[] lexema.value;
            lexema.value = new wchar_t[value.size() + 1];
            std::copy(value.begin(), value.end(), lexema.value);
            lexema.value[value.size()] = L'\0';
        }
        std::wcout << lexema.value << L" type: " << lexema.type << std::endl;
    }

    wcout << L"\n\033[1;37mErrores léxicos\033[0m" << endl;
    for (auto &error : errores)
    {
        std::wcout << error.value << L" at line: " << error.numberLine << L" at column: " << error.numberColumn << std::endl;
    }
    wcout << endl;

    std::vector<Symbol> lexicalAnalyzer;
    std::vector<Symbol> variables;
    std::vector<Symbol> lexErrors;
    // Build the lexical analyzer
    for (int i = 0; i < lexemasAceptados.size(); i++)
    {
        Symbol symbol = lexemasAceptados[i];

        if (i == 0 && wcscmp(symbol.type, L"Opening curly bracket") == 0)
        {
            if (i + 1 < lexemasAceptados.size())
            {
                i++;
            }
            else
            {
                throw std::runtime_error("Unexpected end of file");
            }
            bool closedFounded = false;
            while (i < lexemasAceptados.size())
            {
                if (wcscmp(lexemasAceptados[i].type, L"Closing curly bracket") == 0)
                {
                    closedFounded = true;
                    break;
                }
                lexemasAceptados[i].type = L"Header Statement";
                lexicalAnalyzer.push_back(lexemasAceptados[i]);
                if (i + 1 < lexemasAceptados.size())
                {
                    i++;
                }
                else
                {
                    throw std::runtime_error("Unexpected end of file");
                }
            }
        }
        else if (wcscmp(symbol.type, L"Declaration") == 0)
        {
            if (i + 1 < lexemasAceptados.size())
            {
                i++;
            }
            else
            {
                throw std::runtime_error("Unexpected end of file");
            }
            if (wcscmp(lexemasAceptados[i].type, L"Identifier") == 0)
            {
                Symbol variable;
                variable.type = lexemasAceptados[i].value; // Set the type to the identifier's value
                if (i + 1 < lexemasAceptados.size())
                {
                    i++;
                }
                else
                {
                    throw std::runtime_error("Unexpected end of file");
                }
                if (wcscmp(lexemasAceptados[i].type, L"Assignment") == 0)
                {
                    if (i + 1 < lexemasAceptados.size())
                    {
                        i++;
                    }
                    else
                    {
                        throw std::runtime_error("Unexpected end of file");
                    }
                    if (wcscmp(lexemasAceptados[i].type, L"Opening parenthesis") == 0)
                    {
                        if (i + 1 < lexemasAceptados.size())
                        {
                            i++;
                        }
                        else
                        {
                            throw std::runtime_error("Unexpected end of file");
                        }
                        std::wstring regexValue;
                        while (wcscmp(lexemasAceptados[i].type, L"Closing parenthesis") != 0)
                        {
                            if (wcscmp(lexemasAceptados[i].type, L"Regular Character") == 0)
                            {
                                // Remove single quotes from value
                                std::wstring value = lexemasAceptados[i].value;
                                value.erase(std::remove(value.begin(), value.end(), '\''), value.end());
                                regexValue += value;
                            }
                            else if (wcscmp(lexemasAceptados[i].type, L"String Character") == 0)
                            {
                                // Remove double quotes from value
                                std::wstring value = lexemasAceptados[i].value;
                                value.erase(std::remove(value.begin(), value.end(), '\"'), value.end());
                                regexValue += value;
                            }
                            else if (wcscmp(lexemasAceptados[i].type, L"Identifier") == 0)
                            {
                                wcout << L"Identifier: " << lexemasAceptados[i].value << endl;
                                // Find the variable with the same value as the identifier and use its value
                                for (const Symbol &variable : variables)
                                {
                                    if (wcscmp(variable.type, lexemasAceptados[i].value) == 0)
                                    {
                                        regexValue += variable.value;
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                regexValue += lexemasAceptados[i].value;
                            }

                            if (i + 1 < lexemasAceptados.size())
                            {
                                i++;
                            }
                            else
                            {
                                throw std::runtime_error("Unexpected end of file");
                            }
                        }
                        variable.value = new wchar_t[regexValue.size() + 1];
                        std::copy(regexValue.begin(), regexValue.end(), variable.value);
                        variable.value[regexValue.size()] = L'\0';
                        variables.push_back(variable);
                    }
                    else
                    {
                        lexErrors.push_back(lexemasAceptados[i]);
                    }
                }
                else
                {
                    lexErrors.push_back(lexemasAceptados[i]);
                }
            }
            else
            {
                lexErrors.push_back(lexemasAceptados[i]);
            }
        }
        else if (wcscmp(symbol.type, L"Rule") == 0)
        {
            if (i + 1 < lexemasAceptados.size())
            {
                i++;
            }
            else
            {
                throw std::runtime_error("Unexpected end of file");
            }

            symbol = lexemasAceptados[i];
            wcout << L"Building Rule: " << symbol.value << endl;
            if (i + 1 < lexemasAceptados.size())
            {
                i++;
            }
            else
            {
                throw std::runtime_error("Unexpected end of file");
            }
            if (wcscmp(lexemasAceptados[i].type, L"Opening square bracket") == 0)
            {
                wcout << L"Including params... " << endl;
                if (i + 1 < lexemasAceptados.size())
                {
                    i++;
                }
                else
                {
                    throw std::runtime_error("Unexpected end of file");
                }
                while (wcscmp(lexemasAceptados[i].type, L"Closing square bracket") != 0)
                {
                    wcout << L"Param: " << lexemasAceptados[i].value << endl;
                    if (i + 1 < lexemasAceptados.size())
                    {
                        i++;
                    }
                    else
                    {
                        throw std::runtime_error("Unexpected end of file");
                    }
                }
                if (i + 1 < lexemasAceptados.size())
                {
                    i++;
                }
                else
                {
                    throw std::runtime_error("Unexpected end of file");
                }
            }
            if (wcscmp(lexemasAceptados[i].type, L"Assignment") == 0)
            {
                bool keepGoing = false;

                do
                {
                    if (i + 1 < lexemasAceptados.size())
                    {
                        i++;
                    }
                    else
                    {
                        throw std::runtime_error("Unexpected end of file");
                    }
                    std::wstring regexValue;
                    std::wstring returnType;
                    wcout << endl;
                    wcout << L"Building regex... " << lexemasAceptados[i].value << endl;
                    wcout << endl;
                    if (wcscmp(lexemasAceptados[i].type, L"Opening parenthesis") == 0)
                    {
                        if (i + 1 < lexemasAceptados.size())
                        {
                            i++;
                        }
                        else
                        {
                            throw std::runtime_error("Unexpected end of file");
                        }
                        while (wcscmp(lexemasAceptados[i].type, L"Closing parenthesis") != 0)
                        {

                            if (wcscmp(lexemasAceptados[i].type, L"Regular Character") == 0)
                            {
                                // Remove single quotes from value
                                std::wstring value = lexemasAceptados[i].value;
                                value.erase(std::remove(value.begin(), value.end(), '\''), value.end());
                                regexValue += value;
                            }
                            else if (wcscmp(lexemasAceptados[i].type, L"String Character") == 0)
                            {
                                // Remove double quotes from value
                                std::wstring value = lexemasAceptados[i].value;
                                value.erase(std::remove(value.begin(), value.end(), '\"'), value.end());
                                regexValue += value;
                            }
                            else if (wcscmp(lexemasAceptados[i].type, L"Identifier") == 0)
                            {
                                // Find the variable with the same value as the identifier and use its value
                                for (const Symbol &variable : variables)
                                {
                                    if (wcscmp(variable.type, lexemasAceptados[i].value) == 0)
                                    {
                                        regexValue += variable.value;
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                regexValue += lexemasAceptados[i].value;
                            }

                            if (i + 1 < lexemasAceptados.size())
                            {
                                i++;
                            }
                            else
                            {
                                throw std::runtime_error("Unexpected end of file");
                            }
                        }
                        wcout << L"Regex: " << regexValue << endl;
                    }
                    else if (wcscmp(lexemasAceptados[i].type, L"Identifier") == 0)
                    {
                        wcout << L"Identifier: " << lexemasAceptados[i].value << endl;
                        std::wstring identifierValue;
                        for (const Symbol &variable : variables)
                        {
                            if (wcscmp(variable.type, lexemasAceptados[i].value) == 0)
                            {
                                identifierValue = variable.value;
                                break;
                            }
                        }
                        regexValue = identifierValue;
                        wcout << L"Value: " << identifierValue << endl;
                    }
                    else
                    {
                        lexErrors.push_back(lexemasAceptados[i]);
                    }

                    if (i + 1 < lexemasAceptados.size())
                    {
                        i++;
                    }
                    else
                    {
                        throw std::runtime_error("Unexpected end of file");
                    }
                    if (wcscmp(lexemasAceptados[i].type, L"Opening curly bracket") == 0)
                    {
                        if (i + 1 < lexemasAceptados.size())
                        {
                            i++;
                        }
                        else
                        {
                            throw std::runtime_error("Unexpected end of file");
                        }
                        if (wcscmp(lexemasAceptados[i].type, L"Return Statement") == 0)
                        {
                            if (i + 1 < lexemasAceptados.size())
                            {
                                i++;
                            }
                            else
                            {
                                throw std::runtime_error("Unexpected end of file");
                            }
                            if (wcscmp(lexemasAceptados[i].type, L"Identifier") == 0)
                            {
                                returnType = lexemasAceptados[i].value;
                                wcout << L"Return type: " << returnType << endl;

                                if (i + 1 < lexemasAceptados.size())
                                {
                                    i++;
                                }
                                else
                                {
                                    throw std::runtime_error("Unexpected end of file");
                                }
                            }
                            else
                            {
                                lexErrors.push_back(lexemasAceptados[i]);
                            }
                        }
                        else
                        {
                            lexErrors.push_back(lexemasAceptados[i]);
                        }
                    }
                    else
                    {
                        lexErrors.push_back(lexemasAceptados[i]);
                    }
                    if (!regexValue.empty() && !returnType.empty())
                    {
                        Symbol lexicalRule;
                        lexicalRule.type = new wchar_t[returnType.length() + 1];
                        lexicalRule.value = new wchar_t[regexValue.length() + 1];
                        wcscpy(lexicalRule.type, returnType.c_str());
                        wcscpy(lexicalRule.value, regexValue.c_str());
                        lexicalAnalyzer.push_back(lexicalRule);
                    }
                    if (i + 1 < lexemasAceptados.size())
                    {
                        if (i + 1 < lexemasAceptados.size())
                        {
                            i++;
                        }
                        else
                        {
                            throw std::runtime_error("Unexpected end of file");
                        }
                        if (wcscmp(lexemasAceptados[i].type, L"Vertical bar") == 0)
                        {
                            wcout << L"Keep going... " << lexemasAceptados[i].value << endl;
                            keepGoing = true;
                        }
                        else
                        {
                            keepGoing = false;
                        }
                    }
                    else
                    {
                        keepGoing = false;
                    }
                } while (keepGoing);
            }
            else
            {
                lexErrors.push_back(lexemasAceptados[i]);
            }
        }

        wcout << L"\nLexical Analyzer: " << endl;
        for (auto &symbol : lexicalAnalyzer)
        {
            std::wcout << symbol.value << L" type: " << symbol.type << std::endl;
        }

        /* for (auto &variable : variables)
        {
            std::wcout << variable.value << L" type: " << variable.type << std::endl;
        } */

        try
        {
            for (auto &pattern : lexicalAnalyzer)
            {
                Stack<shuntingToken> postfixRegex = shuntingYard(pattern.value);
                std::wstring alphabet = getAlphabet(&postfixRegex);
                Automata *automata = thompson(constructSyntaxTree(&postfixRegex), alphabet);
                std::wstring type = pattern.type;

                std::transform(type.begin(), type.end(), type.begin(),
                               [](wchar_t c)
                               { return std::towlower(c); });

                std::replace(type.begin(), type.end(), L' ', L'_');

                std::wstring prefix = L"lex_";
                type = prefix + type;

                generateGraph(automata, type);
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Caught exception: " << e.what() << '\n';
        }
        catch (...)
        {
            std::cerr << "Caught unknown exception\n";
        }
    }
}

void MyFrame::OnExit(wxCommandEvent &event)
{
    // Si hay más de una página, cierra la página actual
    if (notebook->GetPageCount() > 1)
    {
        notebook->DeletePage(notebook->GetSelection());
    }
    // Si solo queda una página, cierra la página y la ventana
    else if (notebook->GetPageCount() == 1)
    {
        notebook->DeletePage(notebook->GetSelection());
        Close(true);
    }
    // Si no hay páginas, cierra la ventana
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
            return; // the user changed idea...

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
        return; // the user changed idea...

    // proceed loading the file chosen by the user;
    wxStyledTextCtrl *textCtrl = (wxStyledTextCtrl *)notebook->GetCurrentPage();
    if (textCtrl->GetText().IsEmpty())
    {
        // If current page is empty, overwrite it
        textCtrl->ClearAll();
        notebook->SetPageText(notebook->GetSelection(), openFileDialog.GetFilename());
    }
    else
    {
        // If current page is not empty, create a new page
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

void MyFrame::OnKeyDown(wxKeyEvent &event) // Cambia a este evento
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

        if (selStart != selEnd) // Hay texto seleccionado
        {
            wxString selectedText = textCtrl->GetSelectedText();
            wxString newText = ch + selectedText + close_ch;
            textCtrl->SetSelection(selStart, selEnd); // Restablece la selección
            textCtrl->ReplaceSelection(newText);
            event.Skip(false); // Evita que el evento se procese más
        }
        else // No hay texto seleccionado
        {
            wxString newText = wxString(ch) + close_ch;
            textCtrl->AddText(newText);
            textCtrl->GotoPos(textCtrl->GetCurrentPos() - 1);
            event.Skip(false); // Evita que el evento se procese más
        }
    }
    else
    {
        event.Skip(); // Permite que el evento se procese normalmente
    }
}

/* int main(int argc, char *argv[])
{

    return wxEntry(argc, argv);
} */