#include "RSADialog.h"

RSADialog::RSADialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer(wxHORIZONTAL);

	mStaticTextPublicKeyHint = new wxStaticText(this, wxID_ANY, wxT("选择加密用的公钥"), wxDefaultPosition, wxDefaultSize, 0);
	mStaticTextPublicKeyHint->Wrap(-1);
	bSizer7->Add(mStaticTextPublicKeyHint, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	mFilePickerPublicKey = new wxFilePickerCtrl(this, wxID_ANY, wxEmptyString, wxT("选择加密用的公钥"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE | wxFLP_FILE_MUST_EXIST);
	bSizer7->Add(mFilePickerPublicKey, 1, wxALL, 5);


	bSizer6->Add(bSizer7, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer71;
	bSizer71 = new wxBoxSizer(wxHORIZONTAL);

	mStaticTextPrivateKeyHint = new wxStaticText(this, wxID_ANY, wxT("选择解密用的私钥"), wxDefaultPosition, wxDefaultSize, 0);
	mStaticTextPrivateKeyHint->Wrap(-1);
	bSizer71->Add(mStaticTextPrivateKeyHint, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	mFilePickerPrivateKey = new wxFilePickerCtrl(this, wxID_ANY, wxEmptyString, wxT("选择解密用的私钥"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE | wxFLP_FILE_MUST_EXIST);
	bSizer71->Add(mFilePickerPrivateKey, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);


	bSizer6->Add(bSizer71, 0, wxEXPAND, 5);

	mTextCtrlOrigin = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	mTextCtrlOrigin->SetMinSize(wxSize(615, 200));

	bSizer6->Add(mTextCtrlOrigin, 1, wxALL, 5);

	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer(wxHORIZONTAL);

	mButtonEncrypt = new wxButton(this, wxID_ANY, wxT("加密"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer14->Add(mButtonEncrypt, 1, wxALL, 5);

	mButtonDecrypt = new wxButton(this, wxID_ANY, wxT("解密"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer14->Add(mButtonDecrypt, 1, wxALL, 5);


	bSizer6->Add(bSizer14, 0, wxEXPAND, 5);

	mTextCtrlValue = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
	mTextCtrlValue->SetMinSize(wxSize(615, 250));

	bSizer6->Add(mTextCtrlValue, 1, wxALL, 5);


	this->SetSizer(bSizer6);
	this->Layout();
	bSizer6->Fit(this);

	this->Centre(wxBOTH);

	// Connect Events
	mFilePickerPublicKey->Connect(wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(RSADialog::OnFilePublicKeyChanged), NULL, this);
	mFilePickerPrivateKey->Connect(wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(RSADialog::OnFilePrivateKeyChanged), NULL, this);
	mButtonEncrypt->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(RSADialog::OnButtonEncryptClicked), NULL, this);
	mButtonDecrypt->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(RSADialog::OnButtonDecryptClicked), NULL, this);
}

RSADialog::~RSADialog()
{
	// Disconnect Events
	mFilePickerPublicKey->Disconnect(wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(RSADialog::OnFilePublicKeyChanged), NULL, this);
	mFilePickerPrivateKey->Disconnect(wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(RSADialog::OnFilePrivateKeyChanged), NULL, this);
	mButtonEncrypt->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(RSADialog::OnButtonEncryptClicked), NULL, this);
	mButtonDecrypt->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(RSADialog::OnButtonDecryptClicked), NULL, this);

}
