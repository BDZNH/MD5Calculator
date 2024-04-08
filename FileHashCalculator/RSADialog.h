///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.1.0-0-g733bf3d)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/filepicker.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class RSADialog
///////////////////////////////////////////////////////////////////////////////
class RSADialog : public wxDialog
{
private:

protected:
	wxStaticText* mStaticTextPublicKeyHint;
	wxFilePickerCtrl* mFilePickerPublicKey;
	wxStaticText* mStaticTextPrivateKeyHint;
	wxFilePickerCtrl* mFilePickerPrivateKey;
	wxTextCtrl* mTextCtrlOrigin;
	wxButton* mButtonEncrypt;
	wxButton* mButtonDecrypt;
	wxTextCtrl* mTextCtrlValue;

	// Virtual event handlers, override them in your derived class
	virtual void OnFilePublicKeyChanged(wxFileDirPickerEvent& event) { event.Skip(); }
	virtual void OnFilePrivateKeyChanged(wxFileDirPickerEvent& event) { event.Skip(); }
	virtual void OnButtonEncryptClicked(wxCommandEvent& event) { event.Skip(); }
	virtual void OnButtonDecryptClicked(wxCommandEvent& event) { event.Skip(); }


public:

	RSADialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("RSA/ECB/PKCS1PADDING º”Ω‚√‹"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);

	~RSADialog();

};

