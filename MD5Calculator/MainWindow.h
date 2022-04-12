// For compilers that support precompilation, includes "wx/wx.h".

#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/listctrl.h>
#include <unordered_map>
#include "threadpool.h"
#include "FileUtil.h"
#include "FileHashInfoList.h"
#include "CommonEmnuID.h"
#include <mutex>

class MyFrame : public wxFrame,public ListCtrlCallBack
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    void onMessage(wxString str);
private:
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnOpenFile(wxCommandEvent& event);
    void OnSaveAllFileInfoAs(wxCommandEvent& event);
    void OnDrapFiles(wxDropFilesEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

    void OnCopySelectedFileName(wxCommandEvent& event);
    void OnCopySelectedMd5(wxCommandEvent& event);
    void OnCopySelectedCrc32(wxCommandEvent& event);
    void OnCopySelectedFileInfo(wxCommandEvent& event);
    void OnCopySelectedFileNameAndMd5(wxCommandEvent& event);
    void OnCopyAllFileInfo(wxCommandEvent& event);
    void OnClearItem(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();

    FileHashInfoList* mListCtrl = nullptr;
    std::recursive_mutex mLock;
};

