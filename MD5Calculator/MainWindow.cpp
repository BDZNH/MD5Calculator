#include "MainWindow.h"
#include <openssl/opensslv.h>
#include <wx/versioninfo.h>
#include <wx/dir.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
EVT_MENU(wxID_OPEN, MyFrame::OnOpenFile)
EVT_MENU(wxID_SAVEAS, MyFrame::OnSaveAllFileInfoAs)
EVT_MENU(ID_COPY_SELECTED_FILENAME, MyFrame::OnCopySelectedFileName)
EVT_MENU(ID_COPY_SELECTED_MD5, MyFrame::OnCopySelectedMd5)
EVT_MENU(ID_COPY_SELECTED_CRC32, MyFrame::OnCopySelectedCrc32)
EVT_MENU(ID_COPY_SELECTED_FILENAME_MD5, MyFrame::OnCopySelectedFileNameAndMd5)
EVT_MENU(ID_COPY_SELECTED_FILE_INFO, MyFrame::OnCopySelectedFileInfo)
EVT_MENU(ID_COPY_ALL_ITEM_INFO, MyFrame::OnCopyAllFileInfo)
EVT_MENU(ID_CLEAR_ALL_ITEM,MyFrame::OnClearItem)
wxEND_EVENT_TABLE()

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    SetLabel(title);
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(wxID_OPEN, "&��...\tCtrl-O", "ѡ���ļ��Դ�");
    menuFile->Append(wxID_SAVEAS, "&���Ϊ...\tCtrl-S", "�������ļ�����Ϣ���Ϊ");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, "&�˳�...\tCtrl-Q","�˳�Ӧ��");
    Bind(wxEVT_MENU,&MyFrame::OnExit,this,wxID_EXIT);
    Bind(wxEVT_CLOSE_WINDOW, &MyFrame::OnCloseWindow, this);
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT,"&����...\tCtrl-I","���ڱ�Ӧ��");
    wxMenu* menuEdit = new wxMenu;
    menuEdit->Append(ID_COPY_SELECTED_FILENAME, "&����ѡ������ļ�·��");
    menuEdit->Append(ID_COPY_SELECTED_MD5, "&����ѡ�����MD5");
    menuEdit->Append(ID_COPY_SELECTED_CRC32, "&����ѡ�����CR32");
    menuEdit->Append(ID_COPY_SELECTED_FILENAME_MD5, "&����ѡ�����ļ�����MD5");
    menuEdit->Append(ID_COPY_SELECTED_FILE_INFO, "&����ѡ������ļ���Ϣ");
    menuEdit->Append(ID_COPY_ALL_ITEM_INFO, "&���������ļ�����Ϣ");
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_CLEAR_ALL_ITEM, "&����б�...\tCtrl-L");

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "�ļ�");
    menuBar->Append(menuEdit, "�༭");
    menuBar->Append(menuHelp, "����");
    //SetBackgroundColour({ 240,240,240,0 });
    SetIcon(wxICON(sample));
    SetMenuBar(menuBar);
    CreateStatusBar();
    SetStatusText(title);
    wxSize windowsize = FromDIP(wxSize(800,600));
    wxPanel* mainPanel = new wxPanel(this,wxID_ANY,wxDefaultPosition,windowsize);
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    //mListCtrl = new wxListCtrl(mainPanel, wxID_ANY,wxDefaultPosition, { 800,600 }, wxLC_REPORT);
    mListCtrl = new FileHashInfoList(mainPanel, wxID_ANY, wxDefaultPosition, windowsize, wxLC_REPORT,this, mLock);
    mListCtrl->InsertColumn(0, "���");
    mListCtrl->InsertColumn(1, "�ļ�");
    mListCtrl->InsertColumn(2, "CRC32");
    mListCtrl->InsertColumn(3, "MD5");
    mListCtrl->InsertColumn(4, "״̬");
    int numberwidth = GetDPIScaleFactor() * 50;
    int crcwidth = GetDPIScaleFactor() * 80;
    int md5width = GetDPIScaleFactor() * 270;
    mListCtrl->SetColumnWidth(0, numberwidth);
    mListCtrl->SetColumnWidth(1, md5width);
    mListCtrl->SetColumnWidth(2, crcwidth);
    mListCtrl->SetColumnWidth(3, md5width);
    mListCtrl->SetColumnWidth(4, numberwidth);
    
    mListCtrl->DragAcceptFiles(true);
    mListCtrl->Bind(wxEVT_DROP_FILES, &MyFrame::OnDrapFiles,this,wxID_ANY);
    sizer->Add(mListCtrl, 1, wxEXPAND);
    mainPanel->SetSizerAndFit(sizer);
}
void MyFrame::onMessage(wxString str)
{
    SetStatusText(str);
}
void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MyFrame::OnCloseWindow(wxCloseEvent& event)
{
    std::unique_lock<std::recursive_mutex> _l(mLock);
    if (mListCtrl->IsCalculatFinished())
    {
        delete mListCtrl;
        mListCtrl = nullptr;
        Destroy();
    }
    else
    {
        wxMessageDialog message(this, "�����ļ��ڼ����У�ȷ��Ҫ�˳���", "����", wxOK | wxCANCEL | wxICON_WARNING);
        if (message.ShowModal() == wxID_OK)
        {
            delete mListCtrl;
            mListCtrl = nullptr;
            Destroy();
        }
    }
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg("һ�����׵������鿴�ļ���CRC32У��ֵ��MD5ֵ��С����\n");
    //msg += OPENSSL_VERSION_TEXT;
    //wxVersionInfo version;
    //msg += "\n";
    //msg += version.ToString();
    wxMessageBox(msg,
        "���ڱ�Ӧ��", wxOK | wxICON_INFORMATION);
}
void MyFrame::OnOpenFile(wxCommandEvent& event)
{
    std::unique_lock<std::recursive_mutex> _l(mLock);
    wxFileDialog openFileDialog(this, _("Open XYZ file"), "", "","*", wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }
    wxArrayString paths;
    openFileDialog.GetPaths(paths);
    if (paths.size() > 0)
    {
        wxArrayString files;
        for (size_t i= 0; i < paths.size(); i++)
        {
            if (wxFileExists(paths[i]))
            {
                files.push_back(paths[i]);
            }
        }
        if (files.size() > 0)
        {
            for (size_t i = 0; i < files.size(); i++)
            {
                mListCtrl->InsertItem(files[i]);
            }
        }
        else
        {
            wxMessageBox("û���յ��κ��ļ�","����", wxOK | wxICON_WARNING);
        }
    }

}
void MyFrame::OnSaveAllFileInfoAs(wxCommandEvent& event)
{
    
    if (mListCtrl->GetItemCount() == 0)
    {
        wxMessageBox("��û�д򿪹��κ��ļ�","����", wxOK | wxICON_WARNING);
        return;
    }
    wxFileDialog saveFileDialog(this, _("���������ļ�����Ϣ"), "", "tmp.txt","�ı��ĵ� (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    FILE* file = fopen(saveFileDialog.GetPath().c_str(), "w+");
    if (file)
    {
        std::unique_lock<std::recursive_mutex> _l(mLock);
        mListCtrl->SaveAllFileInfo(file);
        fclose(file);
    }
    else 
    {
        wxMessageBox("��ʧ��", "����", wxOK | wxICON_WARNING);
    }
}
void MyFrame::OnDrapFiles(wxDropFilesEvent& event)
{
    if (event.GetNumberOfFiles() > 0)
    {
        std::unique_lock<std::recursive_mutex> _l(mLock);
        wxString* dropped = event.GetFiles();
        if (!dropped)
            return;

        wxBusyCursor busyCursor;
        wxWindowDisabler disabler;
        wxString name;
        wxArrayString files;

        for (int i = 0; i < event.GetNumberOfFiles(); i++) {
            name = dropped[i];
            if (wxFileExists(name))
                files.push_back(name);
            //else if (wxDirExists(name))
            //    wxDir::GetAllFiles(name, &files);
        }
        if (files.size() > 0)
        {
            for (size_t i = 0; i < files.size(); i++)
            {
                mListCtrl->InsertItem(files[i]);
            }
        }
        else
        {
            wxMessageBox("û���յ��κ��ļ�",
                "About Hello World", wxOK | wxICON_INFORMATION);
        }
    }
    else
    {
        wxMessageBox("û���յ��κ��ļ�",
            "About Hello World", wxOK | wxICON_INFORMATION);
    }
}



void MyFrame::OnCopySelectedFileName(wxCommandEvent& event)
{
    mListCtrl->CopySelectedFilePath(event);
}

void MyFrame::OnCopySelectedMd5(wxCommandEvent& event)
{
    mListCtrl->CopySelectedFileMd5(event);
}

void MyFrame::OnCopySelectedCrc32(wxCommandEvent& event)
{
    mListCtrl->CopySelectedFileCrc32(event);
}

void MyFrame::OnCopySelectedFileInfo(wxCommandEvent& event)
{
    mListCtrl->CopySelectedFileInfo(event);
}

void MyFrame::OnCopySelectedFileNameAndMd5(wxCommandEvent& event)
{
    mListCtrl->CopySelectedFilePathAndMd5(event);
}

void MyFrame::OnCopyAllFileInfo(wxCommandEvent& event)
{
    mListCtrl->CopySelectedAllFileInfo(event);
}

void MyFrame::OnClearItem(wxCommandEvent& event)
{
    mListCtrl->ClearAllItem(event);
}
