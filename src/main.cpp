#include <wx/wx.h>
#include <wx/stc/stc.h>
#include <wx/filedlg.h>
#include <wx/textfile.h>
#include <wx/notebook.h>

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

private:
    wxNotebook *notebook;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_NEW, MyFrame::OnNew)
        EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
            EVT_MENU(wxID_SAVE, MyFrame::OnSave)
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
        /* print2D(tree); */
        TreeNode *treeAugmented = constructSyntaxTree(&postfixAugmented);
        TreeNode *augmentedParsedTree = parseTree(treeAugmented);
        /* print2D(treeAugmented); */
        wstring alphabet = getAlphabet(&postfix);
        /* wcout << "\n----------------------------------------\033[1;37m Por algoritmo McNaughton-Yamada-Thompson \033[0m----------------------------------------" << endl; */
        Automata *mcythompson = thompson(tree, alphabet);
        /* printAutomata(mcythompson); */
        /* generateGraph(mcythompson, L"mcythompson"); */
        /* wcout << "\n----------------------------------------\033[1;37m Por Construccion de Subconjuntos \033[0m----------------------------------------" << endl; */
        Automata *subset = subsetConstruction(mcythompson);
        /* printAutomata(subset); */
        /* generateGraph(subset, L"bysubsets"); */

        /* wcout << "\n----------------------------------------\033[1;37m Por Construccion directa \033[0m----------------------------------------" << endl;
        size_t pos = alphabet.find(L'ε');
        if (pos != std::wstring::npos)
        {
            alphabet.erase(pos, 1);
        }
        Automata *direct = directConstruction(augmentedParsedTree, alphabet);
        completeAFD(direct);
        printAutomata(direct);
        generateGraph(direct, L"bydirect"); */
        /*         wcout << "\n----------------------------------------\033[1;37m Por Construccion de Subconjuntos (Minificado) \033[0m----------------------------------------" << endl;
         */
        Automata *subsetCopy = deepCopyAutomata(subset);
        /* printAutomata(subsetCopy); */
        Automata *minifiedSubset = minifyAutomata(subsetCopy);
        /* printAutomata(minifiedSubset); */
        /* generateGraph(minifiedSubset, L"bysubsetsminified"); */
        /* wcout << "\n----------------------------------------\033[1;37m Por Construccion directa (Minificado) \033[0m----------------------------------------" << endl;
        Automata *minifiedDirect = minifyAutomata(direct);
        printAutomata(minifiedDirect);
        generateGraph(minifiedDirect, L"bydirectminified"); */

        wcout << L"\n\033[1;37mSimulacion de AFD por subconjuntos\033[0m" << endl;
        simulateAutomata(subset, expresion);
        /* wcout << L"\n\033[1;37mSimulacion de AFD por construccion directa\033[0m" << endl;
        simulateAutomata(direct, expresion); */
        wcout << L"\n\033[1;37mSimulacion de AFD por subconjuntos (minificado)\033[0m" << endl;
        simulateAutomata(minifiedSubset, expresion);
        /* wcout << L"\n\033[1;37mSimulacion de AFD por construccion directa (minificado)\033[0m" << endl;
        simulateAutomata(minifiedDirect, expresion); */
        wcout << L"\n\033[1;37mSimulacion de AFN por McNaughton-Yamada-Thompson\033[0m" << endl;
        simulateNFA(mcythompson, expresion);
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

    // Crea una nueva barra de menú
    wxMenuBar *menuBar = new wxMenuBar;

    // Agrega el menú a la barra de menú
    menuBar->Append(fileMenu, "&File");

    // Asigna la barra de menú a la ventana
    SetMenuBar(menuBar);

    // Establece el tamaño inicial de la ventana
    SetSize(wxSize(800, 600));

    // Vincula el evento de guardar al método OnSave
    Bind(wxEVT_MENU, &MyFrame::OnSave, this, wxID_SAVE);
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