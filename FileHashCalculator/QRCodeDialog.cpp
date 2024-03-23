#include "QRCodeDialog.h"

QRCodeDialog::QRCodeDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);

	mTextCtrlQRCodeText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	bSizer2->Add(mTextCtrlQRCodeText, 0, wxALL | wxEXPAND, 5);

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText1 = new wxStaticText(this, wxID_ANY, wxT("容错率（纠错等级）"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1->Wrap(-1);
	bSizer3->Add(m_staticText1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxString mChoiceQRecLevelChoices[] = { wxT("7%"), wxT("15%"), wxT("25%"), wxT("30%") };
	int mChoiceQRecLevelNChoices = sizeof(mChoiceQRecLevelChoices) / sizeof(wxString);
	mChoiceQRecLevel = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, mChoiceQRecLevelNChoices, mChoiceQRecLevelChoices, 0);
	mChoiceQRecLevel->SetSelection(3);
	bSizer3->Add(mChoiceQRecLevel, 0, wxALL, 5);

	m_staticText2 = new wxStaticText(this, wxID_ANY, wxT("版本"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2->Wrap(-1);
	bSizer3->Add(m_staticText2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxString mChoiceQREncodeVersionChoices[] = { wxT("默认"), wxT("21x21"), wxT("25x25"), wxT("29x29"), wxT("33x33"), wxT("37x37"), wxT("41x41"), wxT("45x45"), wxT("49x49"), wxT("53x53"), wxT("57x57"), wxT("61x61"), wxT("65x65"), wxT("69x69"), wxT("73x73"), wxT("77x77"), wxT("81x81"), wxT("85x85"), wxT("89x89"), wxT("93x93"), wxT("97x97"), wxT("101x101"), wxT("105x105"), wxT("109x109"), wxT("113x113"), wxT("117x117"), wxT("121x121"), wxT("125x125"), wxT("129x129"), wxT("133x133"), wxT("137x137"), wxT("141x141"), wxT("145x145"), wxT("149x149"), wxT("153x153"), wxT("157x157"), wxT("161x161"), wxT("165x165"), wxT("169x169"), wxT("173x173"), wxT("177x177") };
	int mChoiceQREncodeVersionNChoices = sizeof(mChoiceQREncodeVersionChoices) / sizeof(wxString);
	mChoiceQREncodeVersion = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, mChoiceQREncodeVersionNChoices, mChoiceQREncodeVersionChoices, 0);
	mChoiceQREncodeVersion->SetSelection(0);
	bSizer3->Add(mChoiceQREncodeVersion, 0, wxALL, 5);

	m_staticText3 = new wxStaticText(this, wxID_ANY, wxT("边框宽度"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText3->Wrap(-1);
	bSizer3->Add(m_staticText3, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	mSpinCtrlPadding = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 99, 30);
	bSizer3->Add(mSpinCtrlPadding, 0, wxALL, 5);

	mCheckBoxScale = new wxCheckBox(this, wxID_ANY, wxT("缩放以适应窗口大小"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer3->Add(mCheckBoxScale, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	mButtonSaveOriginImage = new wxButton(this, wxID_ANY, wxT("保存原图"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer3->Add(mButtonSaveOriginImage, 0, wxALL, 5);


	bSizer2->Add(bSizer3, 0, wxEXPAND, 5);

	mStaticBitMapQrCode = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0);
	mStaticBitMapQrCode->SetMinSize(wxSize(615, 615));

	bSizer2->Add(mStaticBitMapQrCode, 1, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);


	this->SetSizer(bSizer2);
	this->Layout();
	bSizer2->Fit(this);

	this->Centre(wxBOTH);

	// Connect Events
	mTextCtrlQRCodeText->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(QRCodeDialog::OnQrCodeTextEntered), NULL, this);
	mChoiceQRecLevel->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(QRCodeDialog::OnChoiceQrcodeLevel), NULL, this);
	mChoiceQREncodeVersion->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(QRCodeDialog::OnChoiceQREncodeVersion), NULL, this);
	mSpinCtrlPadding->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(QRCodeDialog::OnSpinCtrlPaddingChanged), NULL, this);
	mSpinCtrlPadding->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(QRCodeDialog::OnSpinCtrlPaddingEnter), NULL, this);
	mCheckBoxScale->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(QRCodeDialog::OnCheckBoxScale), NULL, this);
	mButtonSaveOriginImage->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QRCodeDialog::OnButtonSaveOringImageClicked), NULL, this);
}

QRCodeDialog::~QRCodeDialog()
{
	// Disconnect Events
	mTextCtrlQRCodeText->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(QRCodeDialog::OnQrCodeTextEntered), NULL, this);
	mChoiceQRecLevel->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(QRCodeDialog::OnChoiceQrcodeLevel), NULL, this);
	mChoiceQREncodeVersion->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(QRCodeDialog::OnChoiceQREncodeVersion), NULL, this);
	mSpinCtrlPadding->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(QRCodeDialog::OnSpinCtrlPaddingChanged), NULL, this);
	mSpinCtrlPadding->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(QRCodeDialog::OnSpinCtrlPaddingEnter), NULL, this);
	mCheckBoxScale->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(QRCodeDialog::OnCheckBoxScale), NULL, this);
	mButtonSaveOriginImage->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QRCodeDialog::OnButtonSaveOringImageClicked), NULL, this);

}
