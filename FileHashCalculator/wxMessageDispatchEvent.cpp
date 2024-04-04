#include "wxMessageDispatchEvent.h"
wxDEFINE_EVENT(wxEVT_MESSAGE_DISPATCH, wxMessageDispatchEvent);
wxMessageDispatchEvent::wxMessageDispatchEvent(wxEventType eventType, int winId):wxEvent(winId,eventType),what(winId)
{
}

wxMessageDispatchEvent::~wxMessageDispatchEvent()
{
}

wxEvent* wxMessageDispatchEvent::Clone() const
{
    return new wxMessageDispatchEvent(*this);
}
