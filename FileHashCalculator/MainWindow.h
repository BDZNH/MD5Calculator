#pragma once
#include "MainFrame.h"
#include <unordered_map>
#include <map>
#include <mutex>
#include <wx/clipbrd.h>
#include <wx/notifmsg.h>
#include "threadpool.h"
class MainWindow :
    public MainFrame
{
private:
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
	void UpdateFileHash(wxString filepath, long columnid, const wxString& msg);
	void DecreaseColumn(long id);

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

	void OnItemDoubleClicked(wxListEvent& event) wxOVERRIDE;
	void OnItemRightClicked(wxListEvent& event) wxOVERRIDE;
	void OnOpenFileSelected(wxCommandEvent& event) wxOVERRIDE;
	void OnSaveAsSelected(wxCommandEvent& event) wxOVERRIDE;
	void OnExitAppSelected(wxCommandEvent& event) wxOVERRIDE;
	void OnCRC32Selected(wxCommandEvent& event) wxOVERRIDE;
	void OnMD5Selected(wxCommandEvent& event) wxOVERRIDE;
	void OnSha1Selected(wxCommandEvent& event) wxOVERRIDE;
	void OnSha224Selected(wxCommandEvent& event) wxOVERRIDE;
	void OnSha256Selected(wxCommandEvent& event) wxOVERRIDE;
	void OnSha384Selected(wxCommandEvent& event) wxOVERRIDE;
	void OnSha512Selected(wxCommandEvent& event) wxOVERRIDE;
	void OnClearListSelected(wxCommandEvent& event) wxOVERRIDE;
	void OnAboutMeSelected(wxCommandEvent& event) wxOVERRIDE;

	~MainWindow();

public:
    MainWindow(wxWindow* parent);
};

