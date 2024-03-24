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
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class Base64Dialog
///////////////////////////////////////////////////////////////////////////////
class Base64Dialog : public wxDialog
{
private:

protected:
	wxTextCtrl* mTextCtrlOrigin;
	wxButton* mButtonBase64Decode;
	wxButton* mButtonBase64Encode;
	wxTextCtrl* mTextCtrlResult;

	// Virtual event handlers, override them in your derived class
	virtual void OnButtonBase64DecodeClicked(wxCommandEvent& event) { event.Skip(); }
	virtual void OnButtonBase64EncodeClicked(wxCommandEvent& event) { event.Skip(); }


public:

	Base64Dialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Base64±à½âÂë"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);

	~Base64Dialog();

};

