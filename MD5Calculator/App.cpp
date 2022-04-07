// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
// static need link comctl32.lib and Rpcrt4.lib
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"Rpcrt4.lib")
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "MainWindow.h"

class MyApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        if (!wxApp::OnInit())
            return false;
        MyFrame* frame = new MyFrame("ÎÄ¼þhash¼ÆËãÆ÷", wxDefaultPosition,wxDefaultSize);
        frame->SetClientSize(frame->FromDIP(wxSize(800, 600)));
        frame->Center();
        frame->Show(true);
        
        return true;
    }
};


wxIMPLEMENT_APP(MyApp);
