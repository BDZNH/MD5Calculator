#include "MainWindow.h"
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/msgdlg.h>
#include <wx/menu.h>
#include <wx/filedlg.h>
#include <wx/version.h>
#include <vector>
#include <sstream>
#include <openssl/evp.h>
#include <openssl/opensslv.h >
#include "crc32.h"
#include "small.xpm"
#include "QRCodeDialogImpl.h"
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
        std::unique_lock<std::mutex> _l(mMutex);
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
                mListCtrlFileList->InsertItem(itemCount, wxString::Format("%ld",itemCount+1));
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
        EVP_MD_CTX* md5ctx = nullptr;
        EVP_MD_CTX* sha1ctx = nullptr;
        EVP_MD_CTX* sha224ctx = nullptr;
        EVP_MD_CTX* sha256ctx = nullptr;
        EVP_MD_CTX* sha384ctx = nullptr;
        EVP_MD_CTX* sha512ctx = nullptr;

        unsigned char md5value[EVP_MAX_MD_SIZE] = { 0 }, sha1value[EVP_MAX_MD_SIZE] = { 0 }, sha224value[EVP_MAX_MD_SIZE] = { 0 }, sha256value[EVP_MAX_MD_SIZE] = { 0 }, sha384value[EVP_MAX_MD_SIZE] = { 0 }, sha512value[EVP_MAX_MD_SIZE] = { 0 };
        unsigned int md5len = 0, sha1len = 0, sha224len = 0, sha256len = 0, sha384len = 0, sha512len = 0;
        size_t readLen = file.Read(buffer.data(), bufferSize);
        if (exited)
            return;
        if (readLen > 0)
        {
            clock_t before = 0;
            clock_t now = 0;
            if (calmd5)
            {
                md5ctx = EVP_MD_CTX_new();
                EVP_DigestInit_ex2(md5ctx, EVP_get_digestbyname("md5"), NULL);
            }
            if (calsha1)
            {
                sha1ctx = EVP_MD_CTX_new();
                EVP_DigestInit_ex2(sha1ctx, EVP_get_digestbyname("sha1"), NULL);
            }
            if (calsha224)
            {
                sha224ctx = EVP_MD_CTX_new();
                EVP_DigestInit_ex2(sha224ctx, EVP_get_digestbyname("sha224"), NULL);
            }
            if (calsha256)
            {
                sha256ctx = EVP_MD_CTX_new();
                EVP_DigestInit_ex2(sha256ctx, EVP_get_digestbyname("sha256"), NULL);
            }
            if (calsha384)
            {
                sha384ctx = EVP_MD_CTX_new();
                EVP_DigestInit_ex2(sha384ctx, EVP_get_digestbyname("sha384"), NULL);
            }
            if (calsha512)
            {
                sha512ctx = EVP_MD_CTX_new();
                EVP_DigestInit_ex2(sha512ctx, EVP_get_digestbyname("sha512"), NULL);
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
                if (calmd5)
                {
                    EVP_DigestUpdate(md5ctx, buffer.data(), readLen);
                }
                if (calsha1)
                {
                    EVP_DigestUpdate(sha1ctx, buffer.data(), readLen);
                }
                if (calsha224)
                {
                    EVP_DigestUpdate(sha224ctx, buffer.data(), readLen);
                }
                if (calsha256)
                {
                    EVP_DigestUpdate(sha256ctx, buffer.data(), readLen);
                }
                if (calsha384)
                {
                    EVP_DigestUpdate(sha384ctx, buffer.data(), readLen);
                }
                if (calsha512)
                {
                    EVP_DigestUpdate(sha512ctx, buffer.data(), readLen);
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
            if (calmd5)
            {
                EVP_DigestFinal_ex(md5ctx, md5value, &md5len);
                EVP_MD_CTX_free(md5ctx);
                UpdateFileHash(filepath, mFileMD5Column, CharArrayToHexString(md5value, md5len));
            }
            if (calsha1)
            {
                EVP_DigestFinal_ex(sha1ctx, sha1value, &sha1len);
                EVP_MD_CTX_free(sha1ctx);
                UpdateFileHash(filepath, mFileSha1Column, CharArrayToHexString(sha1value, sha1len));
            }
            if (calsha224)
            {
                EVP_DigestFinal_ex(sha224ctx, sha224value, &sha224len);
                EVP_MD_CTX_free(sha224ctx);
                UpdateFileHash(filepath, mFileSha224Column, CharArrayToHexString(sha224value, sha224len));
            }
            if (calsha256)
            {
                EVP_DigestFinal_ex(sha256ctx, sha256value, &sha256len);
                EVP_MD_CTX_free(sha256ctx);
                UpdateFileHash(filepath, mFileSha256Column, CharArrayToHexString(sha256value, sha256len));
            }
            if (calsha384)
            {
                EVP_DigestFinal_ex(sha384ctx, sha384value, &sha384len);
                EVP_MD_CTX_free(sha384ctx);
                UpdateFileHash(filepath, mFileSha384Column, CharArrayToHexString(sha384value, sha384len));
            }
            if (calsha512)
            {
                EVP_DigestFinal_ex(sha512ctx, sha512value, &sha512len);
                EVP_MD_CTX_free(sha512ctx);
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
    std::unique_lock<std::mutex> _l(mMutex);
    mCalculatingFileCount--;
    if (mCalculatingFileCount > 0)
    {
        wxString state = wxString::Format("有 %ld 个文件在计算中", mCalculatingFileCount);
        mStatusBar->SetStatusText(state);
    }
    else if(mCalculatingFileCount == 0)
    {
        mStatusBar->SetStatusText(wxT("全部文件都已经计算完成"));
        mMenuBar->EnableTop(0, true);
        mMenuBar->EnableTop(1, true);
        mMenuBar->EnableTop(2, true);
    }
}

void MainWindow::UpdateFileHash(wxString filepath, long columnid, std::string msg)
{
    if (columnid <= 1)
    {
        return;
    }
    auto iter = mFileListContainers.find(filepath);
    if (iter == mFileListContainers.end())
    {
        return;
    }
    long itemid = iter->second;
    std::unique_lock<std::mutex> _l(mMutex);
    mListCtrlFileList->SetItem(itemid, columnid, msg);
    mListCtrlFileList->SetColumnWidth(columnid, wxLIST_AUTOSIZE);
}

void MainWindow::UpdateFileHash(wxString filepath, long columnid, const wxString& msg)
{
    if (columnid <= 1)
    {
        return;
    }
    auto iter = mFileListContainers.find(filepath);
    if (iter == mFileListContainers.end())
    {
        return;
    }
    long itemid = iter->second;
    std::unique_lock<std::mutex> _l(mMutex);
    mListCtrlFileList->SetItem(itemid, columnid, msg);
    mListCtrlFileList->SetColumnWidth(columnid, wxLIST_AUTOSIZE);
}

void MainWindow::DecreaseColumn(long id)
{
    if (mFileMD5Column > id)
    {
        mFileMD5Column--;
    }
    if (mFileCRC32Column > id)
    {
        mFileCRC32Column--;
    }
    if (mFileSha1Column > id)
    {
        mFileSha1Column--;
    }
    if (mFileSha224Column > id)
    {
        mFileSha224Column--;
    }
    if (mFileSha256Column > id)
    {
        mFileSha256Column--;
    }
    if (mFileSha384Column > id)
    {
        mFileSha384Column--;
    }
    if (mFileSha512Column > id)
    {
        mFileSha512Column--;
    }
    if (mStateColumn > id)
    {
        mStateColumn--;
    }
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
        mStateColumn = mListCtrlFileList->GetColumnCount() - 1;
    }
    else
    {
        mListCtrlFileList->DeleteColumn(mFileCRC32Column);
        DecreaseColumn(mFileCRC32Column);
        mFileCRC32Column = 0;
    }
}

void MainWindow::OnMD5Selected(wxCommandEvent& event) 
{
    if (event.IsChecked())
    {
        mFileMD5Column = mListCtrlFileList->InsertColumn(mStateColumn, wxT("MD5"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mStateColumn = mListCtrlFileList->GetColumnCount() - 1;
    }
    else
    {
        mListCtrlFileList->DeleteColumn(mFileMD5Column);
        DecreaseColumn(mFileMD5Column);
        mFileMD5Column = 0;
    }
}

void MainWindow::OnSha1Selected(wxCommandEvent& event) 
{
    if (event.IsChecked())
    {
        mFileSha1Column = mListCtrlFileList->InsertColumn(mStateColumn, wxT("SHA1"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mStateColumn = mListCtrlFileList->GetColumnCount() - 1;
    }
    else
    {
        mListCtrlFileList->DeleteColumn(mFileSha1Column);
        DecreaseColumn(mFileSha1Column);
        mFileSha1Column = 0;
    }
}

void MainWindow::OnSha224Selected(wxCommandEvent& event) 
{
    if (event.IsChecked())
    {
        mFileSha224Column = mListCtrlFileList->InsertColumn(mStateColumn, wxT("SHA224"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mStateColumn = mListCtrlFileList->GetColumnCount() - 1;
    }
    else
    {
        mListCtrlFileList->DeleteColumn(mFileSha224Column);
        DecreaseColumn(mFileSha224Column);
        mFileSha224Column = 0;
    }
}

void MainWindow::OnSha256Selected(wxCommandEvent& event) 
{
    if (event.IsChecked())
    {
        mFileSha256Column = mListCtrlFileList->InsertColumn(mStateColumn, wxT("SHA256"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mStateColumn = mListCtrlFileList->GetColumnCount() - 1;
    }
    else
    {
        mListCtrlFileList->DeleteColumn(mFileSha256Column);
        DecreaseColumn(mFileSha256Column);
        mFileSha256Column = 0;
    }
}

void MainWindow::OnSha384Selected(wxCommandEvent& event) 
{
    if (event.IsChecked())
    {
        mFileSha384Column = mListCtrlFileList->InsertColumn(mStateColumn, wxT("SHA384"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mStateColumn = mListCtrlFileList->GetColumnCount() - 1;
    }
    else
    {
        mListCtrlFileList->DeleteColumn(mFileSha384Column);
        DecreaseColumn(mFileSha384Column);
        mFileSha384Column = 0;
    }
}

void MainWindow::OnSha512Selected(wxCommandEvent& event) 
{
    if (event.IsChecked())
    {
        mFileSha512Column = mListCtrlFileList->InsertColumn(mStateColumn, wxT("SHA512"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mStateColumn = mListCtrlFileList->GetColumnCount() - 1;
    }
    else
    {
        mListCtrlFileList->DeleteColumn(mFileSha512Column);
        DecreaseColumn(mFileSha512Column);
        mFileSha512Column = 0;
    }
}

void MainWindow::OnClearListSelected(wxCommandEvent& event)
{
    mListCtrlFileList->DeleteAllItems();
    mFileListContainers.clear();
}

void MainWindow::OnQrCodeSelected(wxCommandEvent& event)
{
    QRCodeDialogImpl dialog(this);
    dialog.ShowModal();
}

void MainWindow::OnAboutMeSelected(wxCommandEvent& event)
{
    wxString content = wxT("一个简单的用来计算文件哈希（CRC32/MD5/SHA1/SHA224/SHA256/SHA384/SHA512）的小工具\r\n");
    content.Append(wxT("依赖于\r\n"));
    content.Append(wxVERSION_STRING);
    content.Append("\r\n");
    content.Append(wxT("openssl "));
    content.Append(wxString::Format("%d.%d.%d\r\n", OPENSSL_VERSION_MAJOR, OPENSSL_VERSION_MINOR, OPENSSL_VERSION_PATCH));
    content.Append(wxT("threadpool https://github.com/lzpong/\r\n"));
    content.Append(QRCodeDialogImpl::GetVersion());
    wxMessageBox(content, wxT("关于本应用"), wxOK | wxICON_INFORMATION, this);
}

MainWindow::~MainWindow()
{
    exited = true;
    if (mExector != nullptr)
    {
        delete mExector;
        mExector = nullptr;
    }
    std::map<long, std::string> algorithms;
    algorithms[mFileMD5Column] = "MD5";
    algorithms[mFileCRC32Column] = "CRC32";
    algorithms[mFileSha1Column] = "SHA1";
    algorithms[mFileSha224Column] = "SHA224";
    algorithms[mFileSha256Column] = "SHA256";
    algorithms[mFileSha384Column] = "SHA384";
    algorithms[mFileSha512Column] = "SHA512";
    auto iter = algorithms.begin();
    std::stringstream outstream;
    while (iter != algorithms.end())
    {
        if (iter->first > 0)
        {
            outstream << iter->second << std::endl;
        }
        iter++;
    }
    std::string str = outstream.str();
    if (str != mConfig)
    {
        WriteStdStringToFile("appconfig", str);
    }
    
}
MainWindow::MainWindow(wxWindow* parent):MainFrame(parent, wxID_ANY,wxT("文件哈希计算器"))
{
    SetIcon(wxICON(aaaaaaappIcon));
    mSerialNumberColumn = mListCtrlFileList->AppendColumn(wxT("序号"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    mFilePathColumn = mListCtrlFileList->AppendColumn(wxT("文件路径"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);

    if (wxFileExists("appconfig"))
    {
        mConfig = ReadFileToStdString("appconfig");
        std::stringstream ss(mConfig);
        std::string tmpstr;
        while (ss >> tmpstr)
        {
            if (tmpstr == "CRC32")
            {
                mFileCRC32Column = mListCtrlFileList->AppendColumn(wxT("CRC32"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
                mMenuItemCRC32->Check(true);
            }
            else if (tmpstr == "MD5")
            {
                mFileMD5Column = mListCtrlFileList->AppendColumn(wxT("MD5"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
                mMenuItemMD5->Check(true);
            }
            else if (tmpstr == "SHA1")
            {
                mFileSha1Column = mListCtrlFileList->AppendColumn(wxT("SHA1"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
                mMenuItemsha1->Check(true);
            }
            else if (tmpstr == "SHA224")
            {
                mFileSha224Column = mListCtrlFileList->AppendColumn(wxT("SHA224"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
                mMenuItemsha224->Check(true);
            }
            else if (tmpstr == "SHA256")
            {
                mFileSha256Column = mListCtrlFileList->AppendColumn(wxT("SHA256"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
                mMenuItemsha256->Check(true);
            }
            else if (tmpstr == "SHA384")
            {
                mFileSha384Column = mListCtrlFileList->AppendColumn(wxT("SHA384"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
                mMenuItemsha384->Check(true);
            }
            else if (tmpstr == "SHA512")
            {
                mFileSha512Column = mListCtrlFileList->AppendColumn(wxT("SHA512"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
                mMenuItemsha512->Check(true);
            }
        }
    }
    else
    {
        mFileCRC32Column = mListCtrlFileList->AppendColumn(wxT("CRC32"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mFileMD5Column = mListCtrlFileList->AppendColumn(wxT("MD5"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
        mMenuItemMD5->Check(true);
        mMenuItemCRC32->Check(true);
        std::stringstream instream;
        instream << "CRC32" << std::endl;
        instream << "MD5" << std::endl;
        mConfig = instream.str();
    }

    mStateColumn = mListCtrlFileList->AppendColumn(wxT("状态"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);
    mListCtrlFileList->SetColumnWidth(mSerialNumberColumn, wxLIST_AUTOSIZE_USEHEADER);

    mListCtrlFileList->DragAcceptFiles(true);
    mListCtrlFileList->Bind(wxEVT_DROP_FILES, &MainWindow::OnDrapFiles, this, wxID_ANY);
    this->Center(wxBOTH);
}
