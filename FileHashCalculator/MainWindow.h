#pragma once
#include "MainFrame.h"
#include <unordered_map>
#include <map>
#include <mutex>
#include <wx/clipbrd.h>
#include <wx/notifmsg.h>
#include <wx/config.h>
#include "threadpool.h"
#include "wxMessageDispatchEvent.h"
class MainWindow :
    public MainFrame
{
private:
    wxConfig mAppConfig;
    std::unordered_map<wxString, long> mFileListContainers;
    std::mutex mMutex;

    long mSerialNumberColumn = 0;
    long mFilePathColumn = 0;
    long mFileMD5Column = 0;
    long mFileCRC32Column = 0;
    long mFileSha1Column = 0;
    long mFileSha224Column = 0;
    long mFileSha256Column = 0;
    long mFileSha384Column = 0;
    long mFileSha512Column = 0;
    long mStateColumn = 0;

    long mCalculatingFileCount = 0;

    std::threadpool* mExector = nullptr;

    std::string mConfig;

    bool exited = false;

    wxClipboard mClipBoard;
    wxNotificationMessage mNotification;

    void OnDrapFiles(wxDropFilesEvent& event);
    void CalculatFileHash(wxString filepath);
    void FinishCalculat();
    void UpdateFileHash(wxString filepath, long columnid, std::string msg);
    void UpdateFileHash(wxString filepath, long columnid, wxString msg);

    void OnCopyFilePathClicked(wxCommandEvent& event);
    void OnCopyCRC32Clicked(wxCommandEvent& event);
    void OnCopyMD5Clicked(wxCommandEvent& event);
    void OnCopySha1Clicked(wxCommandEvent& event);
    void OnCopySha224Clicked(wxCommandEvent& event);
    void OnCopySha256Clicked(wxCommandEvent& event);
    void OnCopySha384Clicked(wxCommandEvent& event);
    void OnCopySha512Clicked(wxCommandEvent& event);
    void OnCopyFileHashClicked(wxCommandEvent& event);
    void OnCopyFilePathAndFileHashClicked(wxCommandEvent& event);
    void CopySelectedItemInfo(long column);
    void CopyStringToClipboard(const wxString& msg);
protected:
    virtual void OnItemDoubleClicked(wxListEvent& event) wxOVERRIDE;
    virtual void OnItemRightClicked(wxListEvent& event) wxOVERRIDE;
    virtual void OnOpenFileSelected(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnSaveAsSelected(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnExitAppSelected(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnCRC32Selected(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnMD5Selected(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnSha1Selected(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnSha224Selected(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnSha256Selected(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnSha384Selected(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnSha512Selected(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnClearListSelected(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnMenuItemSwitchCaseSelected(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnQrCodeSelected(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnBase64Selected(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnRSASelected(wxCommandEvent& event) wxOVERRIDE;
    virtual void OnAboutMeSelected(wxCommandEvent& event) wxOVERRIDE;

    void HandleMessage(wxMessageDispatchEvent& event);

public:
    ~MainWindow();
    MainWindow(wxWindow* parent);
    void CalculatFilesHash(const wxArrayString& files);
};

