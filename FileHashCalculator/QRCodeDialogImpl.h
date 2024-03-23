#pragma once
#include "QRCodeDialog.h"
class QRCodeDialogImpl :
    public QRCodeDialog
{
public:
    QRCodeDialogImpl(wxWindow* parent);
    ~QRCodeDialogImpl();
    static wxString GetVersion();
protected:
    virtual void OnQrCodeTextEntered(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnChoiceQrcodeLevel(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnChoiceQREncodeVersion(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnSpinCtrlPaddingChanged(wxSpinEvent& event) wxOVERRIDE;
    virtual void OnSpinCtrlPaddingEnter(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnCheckBoxScale(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnButtonSaveOringImageClicked(wxCommandEvent& event) wxOVERRIDE;
private:
    void GenerateQrCode();
    void UpdateBitmapIfNeed(bool scale);
private:
    std::vector<std::uint8_t> mRealPngData;
};

