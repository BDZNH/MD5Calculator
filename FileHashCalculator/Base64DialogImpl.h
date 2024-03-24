#pragma once
#include "Base64Dialog.h"
class Base64DialogImpl :
    public Base64Dialog
{
public:
    Base64DialogImpl(wxWindow* parent);
    ~Base64DialogImpl();
protected:
    virtual void OnButtonBase64DecodeClicked(wxCommandEvent& event);
    virtual void OnButtonBase64EncodeClicked(wxCommandEvent& event);
private:
private:
};

