#include "MainWindow.h"
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/msgdlg.h>
#include <wx/menu.h>
#include <wx/filedlg.h>
#include <wx/version.h>
#include <wx/aboutdlg.h>
#include <wx/datetime.h>
#include <vector>
#include <sstream>
#include <openssl/evp.h>
#include <openssl/opensslv.h >
#include "crc32.h"
#include "small.xpm"
#include "QRCodeDialogImpl.h"
#include "Base64DialogImpl.h"
#include "RSADialogImpl.h"

#define UPDATE_TEXT_ID wxID_HIGHEST+1
#define UPDATE_STATUS_ID UPDATE_TEXT_ID+1

static long long MapBlockSize(long long filesize)
{
    static long _1KB = 1 * 1024;
    static long _1MB = 1024 * _1KB;
    static long _10MB = 10 * _1MB;
    static long _100MB = 100 * _1MB;
    static long _1GB = 1024 * _1MB;
    if (filesize < _1MB)
    {
        return 4 * _1KB;
    }
    else if (filesize < _10MB)
    {
        return _1MB;
    }
    else if (filesize < _100MB)
    {
        return _10MB;
    }
    else if (filesize < _1GB)
    {
        return _10MB;
    }
    else
    {
        return _100MB;
    }
}

std::string CharArrayToHexString(unsigned char* str, size_t strlength)
{
    size_t bufferSize = strlength * 2 + 1;
    std::vector<char> buffer(bufferSize);
    for (size_t i = 0; i < strlength; i++)
    {
        snprintf(buffer.data() + i * 2, bufferSize - i * 2, "%02X", str[i]);
    }
    return std::move(std::string(buffer.data(), bufferSize-1));
}

std::string ReadFileToStdString(const char* filepath)
{
    struct _stat64 info;
    _stat64(filepath, &info);
    std::vector<char> buffer;
    if (info.st_size > 0)
    {
        buffer.resize(info.st_size);
        FILE* file = fopen(filepath, "rb");
        if (file)
        {
            fread((void*)buffer.data(), 1, info.st_size, file);
            fclose(file);
        }
    }
    return std::move(std::string(buffer.data(), buffer.size()));
}

bool WriteStdStringToFile(const char* filepath, const std::string& msg)
{
    FILE* file = fopen(filepath, "wb");
    if (file)
    {
        fwrite(msg.data(), 1, msg.size(), file);
        fclose(file);
        return true;
    }
    return false;
}

void MainWindow::OnDrapFiles(wxDropFilesEvent& event)
{
    if (event.GetNumberOfFiles() > 0)
    {
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
            {
                files.push_back(name);
            }
        }
        if (files.size() > 0)
        {
            CalculatFilesHash(files);
        }
        else
        {
            wxMessageBox(wxT("没有收到任何文件"),wxT("警告"), wxOK | wxICON_WARNING);
        }
    }
    else
    {
        wxMessageBox(wxT("没有收到任何文件"),wxT("警告"), wxOK | wxICON_WARNING);
    }
}

void MainWindow::CalculatFileHash(wxString filepath)
{
    if (exited)
        return;
    wxFile file(filepath);
    if (file.IsOpened())
    {
        wxFileOffset fileLength = file.Length();
        long long bufferSize = MapBlockSize(fileLength);
        std::vector<unsigned char> buffer(bufferSize);
        
        bool calcrc32 = mMenuItemCRC32->IsChecked();
        bool calmd5 = mMenuItemMD5->IsChecked();
        bool calsha1 = mMenuItemsha1->IsChecked();
        bool calsha224 = mMenuItemsha224->IsChecked();
        bool calsha256 = mMenuItemsha256->IsChecked();
        bool calsha384 = mMenuItemsha384->IsChecked();
        bool calsha512 = mMenuItemsha512->IsChecked();

        uint32_t crcvalue = 0;
        std::shared_ptr<EVP_MD_CTX> md5ctx(EVP_MD_CTX_new(),EVP_MD_CTX_free);
        std::shared_ptr<EVP_MD_CTX> sha1ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free);
        std::shared_ptr<EVP_MD_CTX> sha224ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free);
        std::shared_ptr<EVP_MD_CTX> sha256ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free);
        std::shared_ptr<EVP_MD_CTX> sha384ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free);
        std::shared_ptr<EVP_MD_CTX> sha512ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free);

        unsigned char md5value[EVP_MAX_MD_SIZE] = { 0 }, 
            sha1value[EVP_MAX_MD_SIZE] = { 0 }, 
            sha224value[EVP_MAX_MD_SIZE] = { 0 }, 
            sha256value[EVP_MAX_MD_SIZE] = { 0 }, 
            sha384value[EVP_MAX_MD_SIZE] = { 0 }, 
            sha512value[EVP_MAX_MD_SIZE] = { 0 };
        unsigned int md5len = 0, sha1len = 0, sha224len = 0, sha256len = 0, sha384len = 0, sha512len = 0;
        size_t readLen = file.Read(buffer.data(), bufferSize);
        if (exited)
            return;
        if (readLen > 0)
        {
            clock_t before = 0;
            clock_t now = 0;
            if (calmd5 && md5ctx != nullptr)
            {
                EVP_DigestInit_ex2(md5ctx.get(), EVP_get_digestbyname("md5"), NULL);
            }
            if (calsha1 && sha1ctx != nullptr)
            {
                EVP_DigestInit_ex2(sha1ctx.get(), EVP_get_digestbyname("sha1"), NULL);
            }
            if (calsha224 && sha224ctx != nullptr)
            {
                EVP_DigestInit_ex2(sha224ctx.get(), EVP_get_digestbyname("sha224"), NULL);
            }
            if (calsha256 && sha256ctx != nullptr)
            {
                EVP_DigestInit_ex2(sha256ctx.get(), EVP_get_digestbyname("sha256"), NULL);
            }
            if (calsha384 && sha384ctx != nullptr)
            {
                EVP_DigestInit_ex2(sha384ctx.get(), EVP_get_digestbyname("sha384"), NULL);
            }
            if (calsha512 && sha512ctx != nullptr)
            {
                EVP_DigestInit_ex2(sha512ctx.get(), EVP_get_digestbyname("sha512"), NULL);
            }
            size_t totalReadedSize = 0;
            if (exited)
                return;
            while (readLen > 0)
            {
                totalReadedSize += readLen;
                now = std::clock();
                if ((now - before) > 100)
                {
                    size_t progress = totalReadedSize * 100 / fileLength;
                    before = now;
                    UpdateFileHash(filepath, mStateColumn, wxString::Format("%zu%%",progress));
                }
                if (calcrc32)
                {
                    crcvalue = calculate_crc32(crcvalue, buffer.data(), (unsigned int)readLen);
                }
                if (calmd5 && md5ctx != nullptr)
                {
                    EVP_DigestUpdate(md5ctx.get(), buffer.data(), readLen);
                }
                if (calsha1 && sha1ctx != nullptr)
                {
                    EVP_DigestUpdate(sha1ctx.get(), buffer.data(), readLen);
                }
                if (calsha224 && sha224ctx != nullptr)
                {
                    EVP_DigestUpdate(sha224ctx.get(), buffer.data(), readLen);
                }
                if (calsha256 && sha256ctx != nullptr)
                {
                    EVP_DigestUpdate(sha256ctx.get(), buffer.data(), readLen);
                }
                if (calsha384 && sha384ctx != nullptr)
                {
                    EVP_DigestUpdate(sha384ctx.get(), buffer.data(), readLen);
                }
                if (calsha512 && sha512ctx != nullptr)
                {
                    EVP_DigestUpdate(sha512ctx.get(), buffer.data(), readLen);
                }
                readLen = file.Read(buffer.data(), bufferSize);
                if (exited)
                    return;
            }
            if (exited)
                return;
            if (calcrc32)
            {
                UpdateFileHash(filepath, mFileCRC32Column, wxString::Format("%08X", crcvalue));
            }
            if (calmd5 && md5ctx != nullptr)
            {
                EVP_DigestFinal_ex(md5ctx.get(), md5value, &md5len);
                UpdateFileHash(filepath, mFileMD5Column, CharArrayToHexString(md5value, md5len));
            }
            if (calsha1 && sha1ctx != nullptr)
            {
                EVP_DigestFinal_ex(sha1ctx.get(), sha1value, &sha1len);
                UpdateFileHash(filepath, mFileSha1Column, CharArrayToHexString(sha1value, sha1len));
            }
            if (calsha224 && sha224ctx != nullptr)
            {
                EVP_DigestFinal_ex(sha224ctx.get(), sha224value, &sha224len);
                UpdateFileHash(filepath, mFileSha224Column, CharArrayToHexString(sha224value, sha224len));
            }
            if (calsha256 && sha256ctx != nullptr)
            {
                EVP_DigestFinal_ex(sha256ctx.get(), sha256value, &sha256len);
                UpdateFileHash(filepath, mFileSha256Column, CharArrayToHexString(sha256value, sha256len));
            }
            if (calsha384 && sha384ctx != nullptr)
            {
                EVP_DigestFinal_ex(sha384ctx.get(), sha384value, &sha384len);
                UpdateFileHash(filepath, mFileSha384Column, CharArrayToHexString(sha384value, sha384len));
            }
            if (calsha512 && sha512ctx != nullptr)
            {
                EVP_DigestFinal_ex(sha512ctx.get(), sha512value, &sha512len);
                UpdateFileHash(filepath, mFileSha512Column, CharArrayToHexString(sha512value, sha512len));
            }
            if (exited)
                return;
            UpdateFileHash(filepath, mStateColumn, wxT("100%"));
        }
        else
        {
            //some error
            wxString msg = wxString::Format("读取文件时发生错误: %s", std::strerror(errno));
            UpdateFileHash(filepath, mStateColumn, msg);
        }
    }
    else
    {
        //some error
        wxString msg = wxString::Format("打开文件时发生错误: %s", std::strerror(errno));
        UpdateFileHash(filepath, mStateColumn, msg.ToStdString());
    }
    if (exited)
        return;
    FinishCalculat();
}

void MainWindow::FinishCalculat()
{
    wxMessageDispatchEvent* event = new wxMessageDispatchEvent(wxEVT_COMMAND_TEXT_UPDATED, UPDATE_STATUS_ID);
    wxQueueEvent(this,event);
}

void MainWindow::UpdateFileHash(wxString filepath, long columnid, std::string msg)
{
    UpdateFileHash(filepath, columnid, wxString(msg));
}

void MainWindow::UpdateFileHash(wxString filepath, long columnid, wxString msg)
{
    if (exited)
        return;
    if (columnid <= 1)
    {
        return;
    }
    if (!mAppConfig.ReadBool("UseUpperCase", true))
    {
        msg.MakeLower();
    }
    auto iter = mFileListContainers.find(filepath);
    if (iter == mFileListContainers.end())
    {
        return;
    }
    long itemid = iter->second;
    wxMessageDispatchEvent* event = new wxMessageDispatchEvent(wxEVT_COMMAND_TEXT_UPDATED, UPDATE_TEXT_ID);
    event->str1 = msg;
    event->arg1 = (int)itemid;
    event->arg2 = (columnid);
    wxQueueEvent(this, event);
}

void MainWindow::OnCopyFilePathClicked(wxCommandEvent& event)
{
    CopySelectedItemInfo(mFilePathColumn);
}

void MainWindow::OnCopyCRC32Clicked(wxCommandEvent& event)
{
    CopySelectedItemInfo(mFileCRC32Column);
}

void MainWindow::OnCopyMD5Clicked(wxCommandEvent& event)
{
    CopySelectedItemInfo(mFileMD5Column);
}

void MainWindow::OnCopySha1Clicked(wxCommandEvent& event)
{
    CopySelectedItemInfo(mFileSha1Column);
}

void MainWindow::OnCopySha224Clicked(wxCommandEvent& event)
{
    CopySelectedItemInfo(mFileSha224Column);
}

void MainWindow::OnCopySha256Clicked(wxCommandEvent& event)
{
    CopySelectedItemInfo(mFileSha256Column);
}

void MainWindow::OnCopySha384Clicked(wxCommandEvent& event)
{
    CopySelectedItemInfo(mFileSha384Column);
}

void MainWindow::OnCopySha512Clicked(wxCommandEvent& event)
{
    CopySelectedItemInfo(mFileSha512Column);
}

void MainWindow::OnCopyFileHashClicked(wxCommandEvent& event)
{
    CopySelectedItemInfo(-1);
}

void MainWindow::OnCopyFilePathAndFileHashClicked(wxCommandEvent& event)
{
    CopySelectedItemInfo(-2);
}

void MainWindow::CopySelectedItemInfo(long column)
{
    wxString msg;
    long itemId = -1;
    long msgcount = 0;
    if (column > 0)
    {
        itemId = mListCtrlFileList->GetNextItem(itemId, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        while (itemId != -1)
        {
            if (msgcount++ > 0)
                msg.Append("\r\n");
            msg.Append(mListCtrlFileList->GetItemText(itemId, column));
            itemId = mListCtrlFileList->GetNextItem(itemId, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        }
    }
    else if (column == -1)
    {
        //拷贝文件哈希信息
        itemId = mListCtrlFileList->GetNextItem(itemId, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        while (itemId != -1)
        {
            if (msgcount++ > 0)
                msg.Append("\r\n");
            for (long id = 2; id < mStateColumn; id++)
            {
                msg.Append(mListCtrlFileList->GetItemText(itemId, id));
                msg.Append(" ");
            }
            itemId = mListCtrlFileList->GetNextItem(itemId, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        }
    }
    else if (column == -2)
    {
        //拷贝文件路径和文件哈希信息
        itemId = mListCtrlFileList->GetNextItem(itemId, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        while (itemId != -1)
        {
            if (msgcount++ > 0)
                msg.Append("\r\n");
            for (long id = 1; id < mStateColumn; id++)
            {
                msg.Append(mListCtrlFileList->GetItemText(itemId, id));
                msg.Append(" ");
            }
            itemId = mListCtrlFileList->GetNextItem(itemId, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        }
    }
    CopyStringToClipboard(msg);
}

void MainWindow::CopyStringToClipboard(const wxString& msg)
{
    if (mClipBoard.Open())
    {
        mClipBoard.SetData(new wxTextDataObject(msg));
        mClipBoard.Close();
    }
    else
    {
        wxMessageBox(wxT("打开剪贴板时发生错误"), wxT("警告"), wxOK | wxICON_WARNING, this);
    }
}

void MainWindow::OnItemDoubleClicked(wxListEvent& event)
{
    long itemId = mListCtrlFileList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (itemId != -1)
    {
        wxString content;
        std::map<long, std::string> algorithms;
        algorithms[mFilePathColumn] = "文件路径";
        algorithms[mFileMD5Column] = "MD5";
        algorithms[mFileCRC32Column] = "CRC32";
        algorithms[mFileSha1Column] = "SHA1";
        algorithms[mFileSha224Column] = "SHA224";
        algorithms[mFileSha256Column] = "SHA256";
        algorithms[mFileSha384Column] = "SHA384";
        algorithms[mFileSha512Column] = "SHA512";
        auto iter = algorithms.begin();
        long mscount = 0;
        while (iter != algorithms.end())
        {
            if (iter->first > 0)
            {
                if (mscount++ > 0)
                    content.Append("\r\n");
                content.Append(wxString::Format("%-15s\t", iter->second.c_str()));
                //content.Append(wxT("    "));
                content.Append(mListCtrlFileList->GetItemText(itemId, iter->first));
            }
            iter++;
        }
        wxMessageBox(content, wxT("文件信息"), wxOK | wxICON_INFORMATION, this);
    }
}

void MainWindow::OnItemRightClicked(wxListEvent& event) 
{
    void* data = reinterpret_cast<void*>(event.GetItem().GetData());
    wxMenu menu;
    wxMenuItem* item = new wxMenuItem(&menu, wxID_ANY, wxT("复制文件路径"));
    menu.Append(item);
    menu.Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainWindow::OnCopyFilePathClicked), this, item->GetId());
    size_t count = 0;
    if (mMenuItemCRC32->IsChecked())
    {
        item = new wxMenuItem(&menu, wxID_ANY, wxT("复制CRC32"));
        menu.Append(item);
        menu.Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainWindow::OnCopyCRC32Clicked), this, item->GetId());
        count++;
    }
    if (mMenuItemMD5->IsChecked())
    {
        item = new wxMenuItem(&menu, wxID_ANY, wxT("复制MD5"));
        menu.Append(item);
        menu.Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainWindow::OnCopyMD5Clicked), this, item->GetId());
        count++;
    }
    if (mMenuItemsha1->IsChecked())
    {
        item = new wxMenuItem(&menu, wxID_ANY, wxT("复制SHA1"));
        menu.Append(item);
        menu.Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainWindow::OnCopySha1Clicked), this, item->GetId());
        count++;
    }
    if (mMenuItemsha224->IsChecked())
    {
        item = new wxMenuItem(&menu, wxID_ANY, wxT("复制SHA224"));
        menu.Append(item);
        menu.Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainWindow::OnCopySha224Clicked), this, item->GetId());
        count++;
    }

    if (mMenuItemsha256->IsChecked())
    {
        item = new wxMenuItem(&menu, wxID_ANY, wxT("复制SHA256"));
        menu.Append(item);
        menu.Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainWindow::OnCopySha256Clicked), this, item->GetId());
        count++;
    }
    if (mMenuItemsha384->IsChecked())
    {
        item = new wxMenuItem(&menu, wxID_ANY, wxT("复制SHA384"));
        menu.Append(item);
        menu.Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainWindow::OnCopySha384Clicked), this, item->GetId());
        count++;
    }
    if (mMenuItemsha512->IsChecked())
    {
        item = new wxMenuItem(&menu, wxID_ANY, wxT("复制SHA512"));
        menu.Append(item);
        menu.Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainWindow::OnCopySha512Clicked), this, item->GetId());
        count++;
    }
    if (count > 1)
    {
        item = new wxMenuItem(&menu, wxID_ANY, wxT("复制文件哈希信息"));
        menu.Append(item);
        menu.Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainWindow::OnCopyFileHashClicked), this, item->GetId());
    }
    if (count > 0)
    {
        item = new wxMenuItem(&menu, wxID_ANY, wxT("复制文件路径和文件哈希信息"));
        menu.Append(item);
        menu.Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainWindow::OnCopyFilePathAndFileHashClicked), this, item->GetId());
    }
    PopupMenu(&menu,event.GetPoint());
}

void MainWindow::OnOpenFileSelected(wxCommandEvent& event) 
{
    wxFileDialog openFileDialog(this, wxT("选择想要计算哈希值的文件"), "", "", "*", wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }
    wxArrayString paths;
    openFileDialog.GetPaths(paths);
    if (paths.size() > 0)
    {
        wxArrayString files;
        for (size_t i = 0; i < paths.size(); i++)
        {
            if (wxFileExists(paths[i]))
            {
                files.push_back(paths[i]);
            }
        }
        if (files.size() > 0)
        {
            if (mExector == nullptr)
            {
                int threadNum = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 4;
                mExector = new std::threadpool(threadNum);
            }
            for (size_t i = 0; i < files.size(); i++)
            {
                auto iter = mFileListContainers.find(files[i]);
                if (iter != mFileListContainers.end())
                {
                    continue;
                }
                long itemCount = mListCtrlFileList->GetItemCount();
                mFileListContainers[files[i]] = itemCount;
                mListCtrlFileList->InsertItem(itemCount, wxString::Format("%ld", itemCount + 1));
                mListCtrlFileList->SetItem(itemCount, mFilePathColumn, files[i]);
                mListCtrlFileList->SetColumnWidth(mFilePathColumn, wxLIST_AUTOSIZE_USEHEADER);
                mCalculatingFileCount++;
                mExector->commit(std::bind(&MainWindow::CalculatFileHash, this, files[i]));
            }
            if (mCalculatingFileCount > 0)
            {
                mStatusBar->SetStatusText(wxString::Format("有 %ld 个文件在计算中", mCalculatingFileCount));
                mMenuBar->EnableTop(0, false);
                mMenuBar->EnableTop(1, false);
                mMenuBar->EnableTop(2, false);
            }
        }
        else
        {
            wxMessageBox("没有收到任何文件", "警告", wxOK | wxICON_WARNING);
        }
    }
}

void MainWindow::OnSaveAsSelected(wxCommandEvent& event) 
{
    long itemCount = mListCtrlFileList->GetItemCount();
    if (mListCtrlFileList->GetItemCount() == 0)
    {
        wxMessageBox("还没有打开过任何文件", "警告", wxOK | wxICON_WARNING);
        return;
    }
    wxFileDialog saveFileDialog(this, wxT("保存所有文件的信息"), "", "tmp.txt", "文本文档 (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }
    long msgcount = 0;
    wxString msg;
    for (long itemId = 0; itemId < itemCount; itemId++)
    {
        if (msgcount++ > 0)
            msg.Append("\r\n");
        for (long id = 1; id < mStateColumn; id++)
        {
            msg.Append(mListCtrlFileList->GetItemText(itemId, id));
            msg.Append(" ");
        }
    }
    if (WriteStdStringToFile(saveFileDialog.GetPath().ToStdString().c_str(), msg.ToStdString()))
    {
        mNotification.SetTitle(wxT("保存成功"));
        mNotification.SetParent(this);
        mNotification.SetMessage(wxT("注意"));
        mNotification.SetFlags(wxICON_INFORMATION);
        mNotification.Show();
    }
    else
    {
        mNotification.SetTitle(wxString::Format("保存失败: %s",std::strerror(errno)));
        mNotification.SetParent(this);
        mNotification.SetMessage(wxT("注意"));
        mNotification.SetFlags(wxICON_WARNING);
        mNotification.Show();
    }
}

void MainWindow::OnExitAppSelected(wxCommandEvent& event) 
{
    Destroy();
}

void MainWindow::OnCRC32Selected(wxCommandEvent& event) 
{
    if (event.IsChecked())
    {
        mFileCRC32Column = mListCtrlFileList->InsertColumn(mStateColumn, wxT("CRC32"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mAppConfig.Write(wxT("HashToolEnableCRC32"), true);
    }
    else
    {
        mListCtrlFileList->DeleteColumn(mFileCRC32Column);
        mAppConfig.Write(wxT("HashToolEnableCRC32"), false);
        mFileCRC32Column = 0;
    }
}

void MainWindow::OnMD5Selected(wxCommandEvent& event) 
{
    if (event.IsChecked())
    {
        mFileMD5Column = mListCtrlFileList->InsertColumn(mStateColumn, wxT("MD5"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mAppConfig.Write(wxT("HashToolEnableMD5"), true);
    }
    else
    {
        mListCtrlFileList->DeleteColumn(mFileMD5Column);
        mAppConfig.Write(wxT("HashToolEnableMD5"), false);
        mFileMD5Column = 0;
    }
}

void MainWindow::OnSha1Selected(wxCommandEvent& event) 
{
    if (event.IsChecked())
    {
        mFileSha1Column = mListCtrlFileList->InsertColumn(mStateColumn, wxT("SHA1"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mAppConfig.Write(wxT("HashToolEnableSha1"), true);
    }
    else
    {
        mListCtrlFileList->DeleteColumn(mFileSha1Column);
        mAppConfig.Write(wxT("HashToolEnableSha1"), false);
        mFileSha1Column = 0;
    }
}

void MainWindow::OnSha224Selected(wxCommandEvent& event) 
{
    if (event.IsChecked())
    {
        mFileSha224Column = mListCtrlFileList->InsertColumn(mStateColumn, wxT("SHA224"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mAppConfig.Write(wxT("HashToolEnableSha224"), true);
    }
    else
    {
        mListCtrlFileList->DeleteColumn(mFileSha224Column);
        mAppConfig.Write(wxT("HashToolEnableSha1"), false);
        mFileSha224Column = 0;
    }
}

void MainWindow::OnSha256Selected(wxCommandEvent& event) 
{
    if (event.IsChecked())
    {
        mFileSha256Column = mListCtrlFileList->InsertColumn(mStateColumn, wxT("SHA256"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mAppConfig.Write(wxT("HashToolEnableSha256"), true);
    }
    else
    {
        mListCtrlFileList->DeleteColumn(mFileSha256Column);
        mAppConfig.Write(wxT("HashToolEnableSha256"), true);
        mFileSha256Column = 0;
    }
}

void MainWindow::OnSha384Selected(wxCommandEvent& event) 
{
    if (event.IsChecked())
    {
        mFileSha384Column = mListCtrlFileList->InsertColumn(mStateColumn, wxT("SHA384"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mAppConfig.Write(wxT("HashToolEnableSha384"), true);
    }
    else
    {
        mListCtrlFileList->DeleteColumn(mFileSha384Column);
        mAppConfig.Write(wxT("HashToolEnableSha384"), false);
        mFileSha384Column = 0;
    }
}

void MainWindow::OnSha512Selected(wxCommandEvent& event) 
{
    if (event.IsChecked())
    {
        mFileSha512Column = mListCtrlFileList->InsertColumn(mStateColumn, wxT("SHA512"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mAppConfig.Write(wxT("HashToolEnableSha512"), true);
    }
    else
    {
        mListCtrlFileList->DeleteColumn(mFileSha512Column);
        mAppConfig.Write(wxT("HashToolEnableSha512"), false);
        mFileSha512Column = 0;
    }
}

void MainWindow::OnClearListSelected(wxCommandEvent& event)
{
    mListCtrlFileList->DeleteAllItems();
    mFileListContainers.clear();
}

void MainWindow::OnMenuItemSwitchCaseSelected(wxCommandEvent& event)
{
    mAppConfig.Write("UseUpperCase", event.IsChecked());
    long columnCount = mListCtrlFileList->GetColumnCount();
    long itemCount = mListCtrlFileList->GetItemCount();
    bool useUpperCase = event.IsChecked();
    if (columnCount > 3)
    {
        columnCount--;
        for (long col = 2; col < columnCount; col++)
        {
            for (long item = 0; item < itemCount; item++)
            {
                wxString msg = mListCtrlFileList->GetItemText(item, col);
                if (useUpperCase)
                {
                    msg.UpperCase();
                }
                else
                {
                    msg.MakeLower();
                }
                mListCtrlFileList->SetItem(item, col,msg);
            }
        }
    }
}

void MainWindow::OnQrCodeSelected(wxCommandEvent& event)
{
    QRCodeDialogImpl dialog(this);
    dialog.ShowModal();
}

void MainWindow::OnBase64Selected(wxCommandEvent& event)
{
    Base64DialogImpl dialog(this);
    dialog.ShowModal();
}

void MainWindow::OnRSASelected(wxCommandEvent& event)
{
    RSADialogImpl dialog(this);
    dialog.ShowModal();
}

void MainWindow::OnAboutMeSelected(wxCommandEvent& event)
{
    wxString content = wxT("一个简单的用来计算文件哈希（CRC32/MD5/SHA1/SHA224/SHA256/SHA384/SHA512）和生成二维码的小工具\r\n");
    content.Append(wxT("依赖于以下开源软件开发:\r\n"));
    content.Append(wxT("\t"));
    content.Append(wxVERSION_STRING);
    content.Append("\r\n");
    content.Append(wxT("\t"));
    content.Append(wxT("openssl "));
    content.Append(wxString::Format("%d.%d.%d\r\n", OPENSSL_VERSION_MAJOR, OPENSSL_VERSION_MINOR, OPENSSL_VERSION_PATCH));
    content.Append(wxT("\t"));
    content.Append(wxT("threadpool https://github.com/lzpong/\r\n"));
    content.Append(wxT("\t"));
    content.Append(QRCodeDialogImpl::GetVersion());
    wxAboutDialogInfo descrip;
    descrip.SetWebSite(wxT("https://github.com/BDZNH/MD5Calculator"), wxT("GitHub"));
    wxString copyRight;
    copyRight.Append("2022-");
    copyRight.Append(wxDateTime::Now().Format("%Y"));
    copyRight.Append(". (c)BDZNH");
    descrip.SetCopyright(copyRight);
    descrip.SetDescription(content);
    descrip.SetName(GetTitle());
    wxArrayString developers;
    developers.push_back(wxT("BDZNH: https://github.com/BDZNH"));
    descrip.SetDevelopers(developers);
    wxAboutBox(descrip,this);
}

void MainWindow::HandleMessage(wxMessageDispatchEvent& event)
{
    switch (event.what)
    {
    case UPDATE_STATUS_ID:
    {
        std::unique_lock<std::mutex> _l(mMutex);
        mCalculatingFileCount--;
        if (mCalculatingFileCount > 0)
        {
            wxString state = wxString::Format("有 %ld 个文件在计算中", mCalculatingFileCount);
            mStatusBar->SetStatusText(state);
        }
        else if (mCalculatingFileCount == 0)
        {
            mStatusBar->SetStatusText(wxT("全部文件都已经计算完成"));
            mMenuBar->EnableTop(0, true);
            mMenuBar->EnableTop(1, true);
            mMenuBar->EnableTop(2, true);
        }
    }
    break;
    case UPDATE_TEXT_ID:
    {
        long itemid = event.arg1;
        long columnid = event.arg2;
        mListCtrlFileList->SetItem(itemid, columnid, event.str1);
        mListCtrlFileList->SetColumnWidth(columnid, wxLIST_AUTOSIZE);
    }
    break;
    default:
        break;
    }
}

MainWindow::~MainWindow()
{
    exited = true;
    if (mExector != nullptr)
    {
        delete mExector;
        mExector = nullptr;
    }
    Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxMessageDispatchEventHandler(MainWindow::HandleMessage), NULL, this);
}
MainWindow::MainWindow(wxWindow* parent):MainFrame(parent, wxID_ANY,wxT("文件哈希计算器")), mAppConfig(wxT("文件哈希计算器"))
{
    Connect(wxEVT_COMMAND_TEXT_UPDATED, wxMessageDispatchEventHandler(MainWindow::HandleMessage), NULL, this);
    SetIcon(wxICON(aaaaaaappIcon));
    mSerialNumberColumn = mListCtrlFileList->AppendColumn(wxT("序号"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    mFilePathColumn = mListCtrlFileList->AppendColumn(wxT("文件路径"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);

    if (mAppConfig.ReadBool("UseUpperCase", true))
    {
        mMenuItemUseUpperCase->Check(true);
    }
    if (mAppConfig.ReadBool("HashToolEnableCRC32",true))
    {
        mFileCRC32Column = mListCtrlFileList->AppendColumn(wxT("CRC32"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mMenuItemCRC32->Check(true);
    }
    if (mAppConfig.ReadBool("HashToolEnableMD5",true))
    {
        mFileMD5Column = mListCtrlFileList->AppendColumn(wxT("MD5"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mMenuItemMD5->Check(true);
    }
    if (mAppConfig.ReadBool("HashToolEnableSHA1",false))
    {
        mFileSha1Column = mListCtrlFileList->AppendColumn(wxT("SHA1"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mMenuItemsha1->Check(true);
    }
    if (mAppConfig.ReadBool("HashToolEnableSHA224",false))
    {
        mFileSha224Column = mListCtrlFileList->AppendColumn(wxT("SHA224"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mMenuItemsha224->Check(true);
    }
    if (mAppConfig.ReadBool("HashToolEnableSHA256",false))
    {
        mFileSha256Column = mListCtrlFileList->AppendColumn(wxT("SHA256"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mMenuItemsha256->Check(true);
    }
    if (mAppConfig.ReadBool("HashToolEnableSHA384",false))
    {
        mFileSha384Column = mListCtrlFileList->AppendColumn(wxT("SHA384"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mMenuItemsha384->Check(true);
    }
    if (mAppConfig.ReadBool("HashToolEnableSHA512",false))
    {
        mFileSha512Column = mListCtrlFileList->AppendColumn(wxT("SHA512"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mMenuItemsha512->Check(true);
    }

    mStateColumn = mListCtrlFileList->AppendColumn(wxT("状态"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);
    mListCtrlFileList->SetColumnWidth(mSerialNumberColumn, wxLIST_AUTOSIZE_USEHEADER);

    mListCtrlFileList->DragAcceptFiles(true);
    mListCtrlFileList->Bind(wxEVT_DROP_FILES, &MainWindow::OnDrapFiles, this, wxID_ANY);
    this->Center(wxBOTH);
}

void MainWindow::CalculatFilesHash(const wxArrayString& files)
{
    std::unique_lock<std::mutex> _l(mMutex);
    if (mExector == nullptr)
    {
        int threadNum = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 4;
        mExector = new std::threadpool(threadNum);
    }
    for (size_t i = 0; i < files.size(); i++)
    {
        if (wxFileName(files[i]).IsDir() || !wxFile::Exists(files[i]))
        {
            continue;
        }
        auto iter = mFileListContainers.find(files[i]);
        if (iter != mFileListContainers.end())
        {
            continue;
        }
        long itemCount = mListCtrlFileList->GetItemCount();
        long id = mListCtrlFileList->InsertItem(itemCount, wxString::Format("%ld", itemCount + 1));
        mFileListContainers[files[i]] = id;
        mListCtrlFileList->SetItem(id, mFilePathColumn, files[i]);
        mListCtrlFileList->SetColumnWidth(mFilePathColumn, wxLIST_AUTOSIZE_USEHEADER);
        mCalculatingFileCount++;
        mExector->commit(std::bind(&MainWindow::CalculatFileHash, this, files[i]));
    }
    if (mCalculatingFileCount > 0)
    {
        mStatusBar->SetStatusText(wxString::Format("有 %ld 个文件在计算中", mCalculatingFileCount));
        mMenuBar->EnableTop(0, false);
        mMenuBar->EnableTop(1, false);
        mMenuBar->EnableTop(2, false);
    }
}
