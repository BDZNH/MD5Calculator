#include <wx/wxprec.h>
#include <wx/cmdline.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "MainWindow.h"

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    wxInitAllImageHandlers();
    MainWindow* window = new MainWindow(NULL);
    window->Show(true);
    if (argc > 1)
    {
        wxArrayString args;
        for (int i = 1; i < argc; i++)
        {
            args.push_back(argv[i]);
        }
        window->CalculatFilesHash(args);
    }
    return true;
}