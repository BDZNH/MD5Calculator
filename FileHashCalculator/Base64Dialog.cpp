#include "Base64Dialog.h"

Base64Dialog::Base64Dialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxVERTICAL);

	mTextCtrlOrigin = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	mTextCtrlOrigin->SetMinSize(wxSize(615, 200));

	bSizer4->Add(mTextCtrlOrigin, 0, wxALL, 5);

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer(wxHORIZONTAL);

	mButtonBase64Decode = new wxButton(this, wxID_ANY, wxT("Base64½âÂë"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer5->Add(mButtonBase64Decode, 1, wxALL, 5);

	mButtonBase64Encode = new wxButton(this, wxID_ANY, wxT("Base64±àÂë"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer5->Add(mButtonBase64Encode, 1, wxALL, 5);


	bSizer4->Add(bSizer5, 1, wxEXPAND, 5);

	mTextCtrlResult = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
	mTextCtrlResult->SetMinSize(wxSize(615, 200));

	bSizer4->Add(mTextCtrlResult, 0, wxALL, 5);


	this->SetSizer(bSizer4);
	this->Layout();
	bSizer4->Fit(this);

	this->Centre(wxBOTH);

	// Connect Events
	mButtonBase64Decode->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Base64Dialog::OnButtonBase64DecodeClicked), NULL, this);
	mButtonBase64Encode->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Base64Dialog::OnButtonBase64EncodeClicked), NULL, this);
}

Base64Dialog::~Base64Dialog()
{
	// Disconnect Events
	mButtonBase64Decode->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Base64Dialog::OnButtonBase64DecodeClicked), NULL, this);
	mButtonBase64Encode->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Base64Dialog::OnButtonBase64EncodeClicked), NULL, this);

}
