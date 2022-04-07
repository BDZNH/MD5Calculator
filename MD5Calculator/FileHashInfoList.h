#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/listctrl.h>
#include <wx/clipbrd.h>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <wx/notifmsg.h>
#include <atomic>
#include "threadpool.h"
#include "FileUtil.h"
#include "CommonEmnuID.h"


class ListCtrlCallBack
{
public:
	virtual void onMessage(wxString str) = 0;
};

class FileHashInfoList: public wxListCtrl, public FileUtil
{
public:
	FileHashInfoList(wxWindow* parent,wxWindowID id,const wxPoint& point,const wxSize& size,long style, ListCtrlCallBack* cb);
	~FileHashInfoList();
	long InsertItem(const wxString& label);
	void OnStartCalculate(const wxString& filepath);
	void OnProgress(const wxString& filepath, int progress);
	void OnStopCalculate(const wxString& filepath, const std::string& crc32, const std::string& md5);

	void CopySelectedFilePath(wxCommandEvent& event);
	void CopySelectedFileCrc32(wxCommandEvent& event);
	void CopySelectedFileMd5(wxCommandEvent& event);
	void CopySelectedFilePathAndMd5(wxCommandEvent& event);
	void CopySelectedFileInfo(wxCommandEvent& event);
	void CopySelectedAllFileInfo(wxCommandEvent& event);
	void ClearAllItem(wxCommandEvent& event);
	void SaveAllFileInfo(FILE* file);

	void NotifyStartCalculate();
	bool IsCalculatFinished();
	
private:
	void OnDoubleClick(wxListEvent& event);
	void OnItemSelected(wxListEvent& event);
	void OnItemDeSelected(wxListEvent& event);
	void OnItemRightClick(wxListEvent& event);
	bool GetCellContentsString(long row_number, int column, wxString& cell_contents_string) const;
	void CopySelectedFileInfo(bool copyFilePath, bool copyFileCrc32, bool copyFileMd5);
	bool IsExistSuchFile(const wxString& filepath);

	wxWindow* mParent;
	wxClipboard* mClipBoard = nullptr;
	ListCtrlCallBack* mCallBack = nullptr;
	std::unordered_map<wxString, long> mFilesStateList;
	std::vector<int> mSelectedItem;
	std::threadpool* mExector = nullptr;
	std::mutex mFileListLock;
	size_t mCalculatingFilesCount = 0;
};

