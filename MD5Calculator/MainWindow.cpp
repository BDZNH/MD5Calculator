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
    menuFile->Append(wxID_OPEN, "&打开...\tCtrl-O", "选择文件以打开");
    menuFile->Append(wxID_SAVEAS, "&另存为...\tCtrl-S", "把所有文件的信息另存为");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, "&退出...\tCtrl-Q","退出应用");
    Bind(wxEVT_MENU,&MyFrame::OnExit,this,wxID_EXIT);
    Bind(wxEVT_CLOSE_WINDOW, &MyFrame::OnCloseWindow, this);
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT,"&关于...\tCtrl-I","关于本应用");
    wxMenu* menuEdit = new wxMenu;
    menuEdit->Append(ID_COPY_SELECTED_FILENAME, "&复制选中项的文件路径");
    menuEdit->Append(ID_COPY_SELECTED_MD5, "&复制选中项的MD5");
    menuEdit->Append(ID_COPY_SELECTED_CRC32, "&复制选中项的CR32");
    menuEdit->Append(ID_COPY_SELECTED_FILENAME_MD5, "&复制选中项文件名和MD5");
    menuEdit->Append(ID_COPY_SELECTED_FILE_INFO, "&复制选中项的文件信息");
    menuEdit->Append(ID_COPY_ALL_ITEM_INFO, "&复制所有文件的信息");
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_CLEAR_ALL_ITEM, "&清空列表...\tCtrl-L");

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "文件");
    menuBar->Append(menuEdit, "编辑");
    menuBar->Append(menuHelp, "帮助");
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
    mListCtrl->InsertColumn(0, "序号");
    mListCtrl->InsertColumn(1, "文件");
    mListCtrl->InsertColumn(2, "CRC32");
    mListCtrl->InsertColumn(3, "MD5");
    mListCtrl->InsertColumn(4, "状态");
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
        wxMessageDialog message(this, "还有文件在计算中，确定要退出吗", "警告", wxOK | wxCANCEL | wxICON_WARNING);
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
    wxString msg("一个简易的用来查看文件的CRC32校验值和MD5值的小工具\n");
    //msg += OPENSSL_VERSION_TEXT;
    //wxVersionInfo version;
    //msg += "\n";
    //msg += version.ToString();
    wxMessageBox(msg,
        "关于本应用", wxOK | wxICON_INFORMATION);
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
            wxMessageBox("没有收到任何文件","警告", wxOK | wxICON_WARNING);
        }
    }

}
void MyFrame::OnSaveAllFileInfoAs(wxCommandEvent& event)
{
    
    if (mListCtrl->GetItemCount() == 0)
    {
        wxMessageBox("还没有打开过任何文件","警告", wxOK | wxICON_WARNING);
        return;
    }
    wxFileDialog saveFileDialog(this, _("保存所有文件的信息"), "", "tmp.txt","文本文档 (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
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
        wxMessageBox("打开失败", "警告", wxOK | wxICON_WARNING);
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
            wxMessageBox("没有收到任何文件",
                "About Hello World", wxOK | wxICON_INFORMATION);
        }
    }
    else
    {
        wxMessageBox("没有收到任何文件",
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
