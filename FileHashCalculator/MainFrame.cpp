///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "MainFrame.h"

MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* mMainBoxSizer;
	mMainBoxSizer = new wxBoxSizer(wxVERTICAL);

	mListCtrlFileList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
	mMainBoxSizer->Add(mListCtrlFileList, 1, wxALL | wxEXPAND, 1);


	this->SetSizer(mMainBoxSizer);
	this->Layout();
	mMenuBar = new wxMenuBar(0);
	mMenuFile = new wxMenu();
	wxMenuItem* mMenuItemFileOpen;
	mMenuItemFileOpen = new wxMenuItem(mMenuFile, wxID_ANY, wxString(wxT("打开文件")) + wxT('\t') + wxT("Ctrl+O"), wxT("打开文件并计算"), wxITEM_NORMAL);
	mMenuFile->Append(mMenuItemFileOpen);

	wxMenuItem* mMenuItemFileSaveInfoAs;
	mMenuItemFileSaveInfoAs = new wxMenuItem(mMenuFile, wxID_ANY, wxString(wxT("另存为")) + wxT('\t') + wxT("CTRL+SHIFT+S"), wxT("将当前的列表信息保存为文本"), wxITEM_NORMAL);
	mMenuFile->Append(mMenuItemFileSaveInfoAs);

	mMenuFile->AppendSeparator();

	wxMenuItem* mMenuItemExit;
	mMenuItemExit = new wxMenuItem(mMenuFile, wxID_ANY, wxString(wxT("退出")) + wxT('\t') + wxT("CTRL+Q"), wxT("退出应用"), wxITEM_NORMAL);
	mMenuFile->Append(mMenuItemExit);

	mMenuBar->Append(mMenuFile, wxT("文件"));

	mMenuEdit = new wxMenu();
	mMenuEditAddAlgorithm = new wxMenu();
	wxMenuItem* mMenuEditAddAlgorithmItem = new wxMenuItem(mMenuEdit, wxID_ANY, wxT("添加"), wxEmptyString, wxITEM_NORMAL, mMenuEditAddAlgorithm);
	mMenuItemCRC32 = new wxMenuItem(mMenuEditAddAlgorithm, wxID_ANY, wxString(wxT("CRC32")), wxT("添加/移除计算CRC32的选项"), wxITEM_CHECK);
	mMenuEditAddAlgorithm->Append(mMenuItemCRC32);

	mMenuItemMD5 = new wxMenuItem(mMenuEditAddAlgorithm, wxID_ANY, wxString(wxT("MD5")), wxT("添加/移除计算MD5的选项"), wxITEM_CHECK);
	mMenuEditAddAlgorithm->Append(mMenuItemMD5);

	mMenuItemsha1 = new wxMenuItem(mMenuEditAddAlgorithm, wxID_ANY, wxString(wxT("Sha1")), wxT("添加/移除计算Sha1的选项"), wxITEM_CHECK);
	mMenuEditAddAlgorithm->Append(mMenuItemsha1);

	mMenuItemsha224 = new wxMenuItem(mMenuEditAddAlgorithm, wxID_ANY, wxString(wxT("Sha224")), wxT("添加/移除计算Sha224的选项"), wxITEM_CHECK);
	mMenuEditAddAlgorithm->Append(mMenuItemsha224);

	mMenuItemsha256 = new wxMenuItem(mMenuEditAddAlgorithm, wxID_ANY, wxString(wxT("Sha256")), wxT("添加/移除计算Sha256的选项"), wxITEM_CHECK);
	mMenuEditAddAlgorithm->Append(mMenuItemsha256);

	mMenuItemsha384 = new wxMenuItem(mMenuEditAddAlgorithm, wxID_ANY, wxString(wxT("Sha384")), wxT("添加/移除计算Sha384的选项"), wxITEM_CHECK);
	mMenuEditAddAlgorithm->Append(mMenuItemsha384);

	mMenuItemsha512 = new wxMenuItem(mMenuEditAddAlgorithm, wxID_ANY, wxString(wxT("Sha512")), wxT("添加/移除计算Sha512的选项"), wxITEM_CHECK);
	mMenuEditAddAlgorithm->Append(mMenuItemsha512);

	mMenuEdit->Append(mMenuEditAddAlgorithmItem);

	wxMenuItem* mMenuItemClear;
	mMenuItemClear = new wxMenuItem(mMenuEdit, wxID_ANY, wxString(wxT("清空列表")), wxEmptyString, wxITEM_NORMAL);
	mMenuEdit->Append(mMenuItemClear);

	mMenuItemUseUpperCase = new wxMenuItem(mMenuEdit, wxID_ANY, wxString(wxT("使用大写")), wxEmptyString, wxITEM_CHECK);
	mMenuEdit->Append(mMenuItemUseUpperCase);

	mMenuBar->Append(mMenuEdit, wxT("编辑"));

	mMenuTools = new wxMenu();
	wxMenuItem* mMenuItemQRCode;
	mMenuItemQRCode = new wxMenuItem(mMenuTools, wxID_ANY, wxString(wxT("二维码生成器")), wxEmptyString, wxITEM_NORMAL);
	mMenuTools->Append(mMenuItemQRCode);

	wxMenuItem* mMenuItemBase64;
	mMenuItemBase64 = new wxMenuItem(mMenuTools, wxID_ANY, wxString(wxT("Base64编解码")), wxEmptyString, wxITEM_NORMAL);
	mMenuTools->Append(mMenuItemBase64);

	wxMenuItem* mMenuItemRSA;
	mMenuItemRSA = new wxMenuItem(mMenuTools, wxID_ANY, wxString(wxT("RSA/ECB/PKCS1PADDING 加解密")), wxEmptyString, wxITEM_NORMAL);
	mMenuTools->Append(mMenuItemRSA);

	mMenuBar->Append(mMenuTools, wxT("小工具"));

	mMenuHelp = new wxMenu();
	wxMenuItem* mMenuItemAbout;
	mMenuItemAbout = new wxMenuItem(mMenuHelp, wxID_ANY, wxString(wxT("关于")), wxT("关于本应用"), wxITEM_NORMAL);
	mMenuHelp->Append(mMenuItemAbout);

	mMenuBar->Append(mMenuHelp, wxT("帮助"));

	this->SetMenuBar(mMenuBar);

	mStatusBar = this->CreateStatusBar(1, wxSTB_SIZEGRIP, wxID_ANY);

	this->Centre(wxBOTH);

	// Connect Events
	mListCtrlFileList->Connect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(MainFrame::OnItemDoubleClicked), NULL, this);
	mListCtrlFileList->Connect(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler(MainFrame::OnItemRightClicked), NULL, this);
	mMenuFile->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnOpenFileSelected), this, mMenuItemFileOpen->GetId());
	mMenuFile->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSaveAsSelected), this, mMenuItemFileSaveInfoAs->GetId());
	mMenuFile->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnExitAppSelected), this, mMenuItemExit->GetId());
	mMenuEditAddAlgorithm->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnCRC32Selected), this, mMenuItemCRC32->GetId());
	mMenuEditAddAlgorithm->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnMD5Selected), this, mMenuItemMD5->GetId());
	mMenuEditAddAlgorithm->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSha1Selected), this, mMenuItemsha1->GetId());
	mMenuEditAddAlgorithm->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSha224Selected), this, mMenuItemsha224->GetId());
	mMenuEditAddAlgorithm->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSha256Selected), this, mMenuItemsha256->GetId());
	mMenuEditAddAlgorithm->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSha384Selected), this, mMenuItemsha384->GetId());
	mMenuEditAddAlgorithm->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSha512Selected), this, mMenuItemsha512->GetId());
	mMenuEdit->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnClearListSelected), this, mMenuItemClear->GetId());
	mMenuEdit->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnMenuItemSwitchCaseSelected), this, mMenuItemUseUpperCase->GetId());
	mMenuTools->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnQrCodeSelected), this, mMenuItemQRCode->GetId());
	mMenuTools->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnBase64Selected), this, mMenuItemBase64->GetId());
	mMenuTools->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnRSASelected), this, mMenuItemRSA->GetId());
	mMenuHelp->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnAboutMeSelected), this, mMenuItemAbout->GetId());
}

MainFrame::~MainFrame()
{
	// Disconnect Events
	mListCtrlFileList->Disconnect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(MainFrame::OnItemDoubleClicked), NULL, this);
	mListCtrlFileList->Disconnect(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler(MainFrame::OnItemRightClicked), NULL, this);

}
