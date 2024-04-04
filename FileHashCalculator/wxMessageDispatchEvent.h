#pragma once
#include <wx/wx.h>
#include <wx/event.h>
#include <wx/string.h>
class wxMessageDispatchEvent :
    public wxEvent
{
public:
    wxMessageDispatchEvent(wxEventType eventType, int winId);
    ~wxMessageDispatchEvent();
    virtual wxEvent* Clone() const;
public:
    int what = 0;
    int arg1 = 0;
    int arg2 = 0;
    int arg3 = 0;
    wxString str1;
    wxString str2;
    wxString str3;
};


wxDECLARE_EVENT(wxEVT_MESSAGE_DISPATCH, wxMessageDispatchEvent);

typedef void (wxEvtHandler::* wxMessageDispatchEventFunction)(wxMessageDispatchEvent&);
#define wxMessageDispatchEventHandler(func) wxEVENT_HANDLER_CAST(wxMessageDispatchEventFunction, func)