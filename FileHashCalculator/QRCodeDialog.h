///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class QRCodeDialog
///////////////////////////////////////////////////////////////////////////////
class QRCodeDialog : public wxDialog
{
private:

protected:
	wxTextCtrl* mTextCtrlQRCodeText;
	wxStaticText* m_staticText1;
	wxChoice* mChoiceQRecLevel;
	wxStaticText* m_staticText2;
	wxChoice* mChoiceQREncodeVersion;
	wxStaticText* m_staticText3;
	wxSpinCtrl* mSpinCtrlPadding;
	wxCheckBox* mCheckBoxScale;
	wxButton* mButtonSaveOriginImage;
	wxStaticBitmap* mStaticBitMapQrCode;

	// Virtual event handlers, override them in your derived class
	virtual void OnQrCodeTextEntered(wxCommandEvent& event) { event.Skip(); }
	virtual void OnChoiceQrcodeLevel(wxCommandEvent& event) { event.Skip(); }
	virtual void OnChoiceQREncodeVersion(wxCommandEvent& event) { event.Skip(); }
	virtual void OnSpinCtrlPaddingChanged(wxSpinEvent& event) { event.Skip(); }
	virtual void OnSpinCtrlPaddingEnter(wxCommandEvent& event) { event.Skip(); }
	virtual void OnCheckBoxScale(wxCommandEvent& event) { event.Skip(); }
	virtual void OnButtonSaveOringImageClicked(wxCommandEvent& event) { event.Skip(); }


public:

	QRCodeDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("¶þÎ¬ÂëÉú³ÉÆ÷"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);

	~QRCodeDialog();

};

