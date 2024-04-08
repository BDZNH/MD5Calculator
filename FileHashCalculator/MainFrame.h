///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.1.0-0-g733bf3d)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame
{
private:

protected:
	wxListCtrl* mListCtrlFileList;
	wxMenuBar* mMenuBar;
	wxMenu* mMenuFile;
	wxMenu* mMenuEdit;
	wxMenu* mMenuEditAddAlgorithm;
	wxMenuItem* mMenuItemCRC32;
	wxMenuItem* mMenuItemMD5;
	wxMenuItem* mMenuItemsha1;
	wxMenuItem* mMenuItemsha224;
	wxMenuItem* mMenuItemsha256;
	wxMenuItem* mMenuItemsha384;
	wxMenuItem* mMenuItemsha512;
	wxMenu* mMenuTools;
	wxMenu* mMenuHelp;
	wxStatusBar* mStatusBar;

	// Virtual event handlers, override them in your derived class
	virtual void OnItemDoubleClicked(wxListEvent& event) { event.Skip(); }
	virtual void OnItemRightClicked(wxListEvent& event) { event.Skip(); }
	virtual void OnOpenFileSelected(wxCommandEvent& event) { event.Skip(); }
	virtual void OnSaveAsSelected(wxCommandEvent& event) { event.Skip(); }
	virtual void OnExitAppSelected(wxCommandEvent& event) { event.Skip(); }
	virtual void OnCRC32Selected(wxCommandEvent& event) { event.Skip(); }
	virtual void OnMD5Selected(wxCommandEvent& event) { event.Skip(); }
	virtual void OnSha1Selected(wxCommandEvent& event) { event.Skip(); }
	virtual void OnSha224Selected(wxCommandEvent& event) { event.Skip(); }
	virtual void OnSha256Selected(wxCommandEvent& event) { event.Skip(); }
	virtual void OnSha384Selected(wxCommandEvent& event) { event.Skip(); }
	virtual void OnSha512Selected(wxCommandEvent& event) { event.Skip(); }
	virtual void OnClearListSelected(wxCommandEvent& event) { event.Skip(); }
	virtual void OnQrCodeSelected(wxCommandEvent& event) { event.Skip(); }
	virtual void OnBase64Selected(wxCommandEvent& event) { event.Skip(); }
	virtual void OnRSASelected(wxCommandEvent& event) { event.Skip(); }
	virtual void OnAboutMeSelected(wxCommandEvent& event) { event.Skip(); }


public:

	MainFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(1280, 720), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

	~MainFrame();

};

