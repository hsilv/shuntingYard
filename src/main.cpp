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
#include "utils.h"

Stack<shuntingToken> postfix;
Stack<shuntingToken> postfixAugmented;
Automata *automatas;

vector<Automata *> addRegex(vector<Automata *> automatas, wstring regex, wstring returnType)
{
    postfix = shuntingYard(regex.c_str());
    TreeNode *tree = constructSyntaxTree(&postfix);
    wstring alphabet = getAlphabet(&postfix);
    Automata *mcythompson = thompson(tree, alphabet);
    mcythompson = addReturnType(mcythompson, returnType);
    automatas.push_back(mcythompson);
    return automatas;
}

Automata *configAutomatas()
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
    automatas = addRegex(automatas, L"\"([a-z]|[A-Z]|[0-9]|\\\\|_|\\*|\\?|\\+|\\.|\\-|\\>|\\<|\\=)+\"", L"String Character");
    automatas = addRegex(automatas, L"([a-z]|[A-Z])+([a-z]|[A-Z]|[0-9])*", L"Identifier");
    automatas = addRegex(automatas, L"entrypoint", L"Entrypoint");
    automatas = addRegex(automatas, L"[0-9]+", L"Integer Number");
    automatas = addRegex(automatas, L"[0-9]+(\\.[0-9]+)?", L"Decimal Number");
    automatas = addRegex(automatas, L"eof", L"End of File");
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

/* wstring evalRegex(wstring regex, wstring input)
{
    postfix = shuntingYard(regex.c_str());
    TreeNode *tree = constructSyntaxTree(&postfix);
    wstring alphabet = getAlphabet(&postfix);
    Automata *mcythompson = thompson(tree, alphabet);
    Automata *subset = subsetConstruction(mcythompson);
    Automata *subsetCopy = deepCopyAutomata(subset);
    Automata *minifiedSubset = minifyAutomata(subsetCopy);
    return simulateNFA(mcythompson, input);
} */

bool MyApp::OnInit()
{
    automatas = configAutomatas();
    printAutomata(automatas);
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
    /* auto start = chrono::high_resolution_clock::now(); */

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
    /* auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
    wstringstream wss;
    wss << elapsed.count();
    wstring elapsed_wstr = wss.str();
    wcout << L"\n\033[1;32mElapsed time: " << elapsed_wstr << L" ms\n\033[0m\n"; */
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
    vector<Symbol *> SymbolTable = vector<Symbol *>();
    vector<Symbol *> ErrorTable = vector<Symbol *>();
    wxStyledTextCtrl *textCtrl = (wxStyledTextCtrl *)notebook->GetCurrentPage();
    wxString text = textCtrl->GetText();
    wstring strText(text.wc_str());

    wstring cleanedText;

    wstring::iterator begin = strText.begin();
    wstring::iterator end = strText.end();
    wstring::iterator current = strText.begin();
    wstring::iterator latestAccepted = strText.begin();
    wstring::iterator latestRejected = strText.begin();
    int numberLine = 1;
    int numberCol = 1;

    wstring latestAcceptedSymbol = L"";

    while (strText.length() > 0)
    {
        while (current != strText.end())
        {
            wstring substring = strText.substr(0, distance(begin, current + 1));
            wstring result = simulateNFA(automatas, substring);

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

                /*  wcout << substring << L" " << result << endl; */
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
            /* wcout << L"Latest Accepted: " << acceptedSubstring << L" " << latestAcceptedSymbol << endl; */
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
                /* wcout << L"bias: " << bias << endl; */
                int startSubs = distance(beginRejected, evalCursor);

                /* wcout << L"startSubs: " << startSubs << endl; */
                for (int j = 1; j <= bias; j++)
                {
                    wstring substringEval = strText.substr(startSubs, j);
                    /* wcout << j << substringEval << endl; */
                    wstring resultSim = simulateNFA(automatas, substringEval);

                    if (resultSim != L"")
                    {
                        refusedToken = strText.substr(0, startSubs);
                        /* wcout << L"Refused: " << refusedToken << endl; */
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
                wstring resultSim = simulateNFA(automatas, substringEval);

                if (resultSim != L"")
                {
                    refusedToken = strText.substr(0, distance(beginRejected, evalCursor));
                    /* wcout << L"Refused: " << refusedToken << endl; */
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
                /* wcout << L"Latest Rejected: " << rejectedSubstring << L" " << L"Rejected" << endl; */
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
                /* wcout << L"Latest Rejected: " << refusedToken << L" " << L"Rejected" << endl; */
            }
        }

        if (current == strText.end())
        {
            break;
        }
    }

    wcout << endl;
    wcout << "Symbol Table" << endl;
    for (Symbol *symbol : SymbolTable)
    {
        wcout << L"Token: " << symbol->value << L" Type: " << symbol->type << L" Line: " << symbol->numberLine << L" Column: " << symbol->numberColumn << endl;
    }
    wcout << endl;

    wcout << "Error Table" << endl;
    for (Symbol *symbol : ErrorTable)
    {
        wcout << L"Token: " << symbol->value << L" Type: " << symbol->type << L" Line: " << symbol->numberLine << L" Column: " << symbol->numberColumn << endl;
    }

    wcout << cleanedText << endl;

    /* generateGraph(automatas, L"newAutomata"); */
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