#pragma once
#include "RSADialog.h"
#include <string>
class RSADialogImpl :
    public RSADialog
{
public:
    RSADialogImpl(wxWindow* parent);

protected:
    virtual void OnFilePublicKeyChanged(wxFileDirPickerEvent& event) wxOVERRIDE;
    virtual void OnFilePrivateKeyChanged(wxFileDirPickerEvent& event) wxOVERRIDE;
    virtual void OnButtonEncryptClicked(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnButtonDecryptClicked(wxCommandEvent& event) wxOVERRIDE;
private:
    void DoEncrypt();
    void DoDecrypt();
private:
    std::string mStringPublicKey;
    std::string mStringPrivateKey;
};

