#include<wx/msgdlg.h>
#include "FileHashInfoList.h"

FileHashInfoList::FileHashInfoList(wxWindow* parent, wxWindowID id, const wxPoint& point, const wxSize& size,long style,ListCtrlCallBack* cb, std::recursive_mutex& globalMutex)
	:wxListCtrl(parent, id, point, size, style), mFileListLock(globalMutex)
{
	mCallBack = cb;
	mParent = parent;
	mClipBoard = new wxClipboard();
	Bind(wxEVT_LIST_ITEM_ACTIVATED, &FileHashInfoList::OnDoubleClick, this, wxID_ANY);
	Bind(wxEVT_LIST_ITEM_SELECTED, &FileHashInfoList::OnItemSelected, this, wxID_ANY);
	Bind(wxEVT_LIST_ITEM_DESELECTED, &FileHashInfoList::OnItemDeSelected, this, wxID_ANY);
	Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &FileHashInfoList::OnItemRightClick, this,wxID_ANY);

	Bind(wxEVT_MENU, &FileHashInfoList::CopySelectedFilePath, this, ID_COPY_FILE_PATH);
	Bind(wxEVT_MENU, &FileHashInfoList::CopySelectedFileCrc32, this, ID_COPY_CRC32);
	Bind(wxEVT_MENU, &FileHashInfoList::CopySelectedFileMd5, this, ID_COPY_MD5);
	Bind(wxEVT_MENU, &FileHashInfoList::CopySelectedFilePathAndMd5, this, ID_COPY_FILE_PATH_MD5);
	Bind(wxEVT_MENU, &FileHashInfoList::CopySelectedFileInfo, this, ID_COPY_FILE_PATH_CRC32_MD5);
}

FileHashInfoList::~FileHashInfoList()
{
	std::unique_lock<std::recursive_mutex> _l(mFileListLock);
	if (mExector != nullptr)
	{
		delete mExector;
		mExector = nullptr;
	}
	size_t size = mSelectedItem.size();
	for (size_t i = 0; i < size; i++)
	{
		mSelectedItem[i] = 0;
	}
	mFilesStateList.clear();
	DeleteAllItems();
}

long FileHashInfoList::InsertItem(const wxString& label)
{
	std::unique_lock<std::recursive_mutex> _l(mFileListLock);
	if (mExector == nullptr)
	{
		int threadNum = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 4;
		mExector = new std::threadpool(threadNum);
	}
	if (IsExistSuchFile(label))
	{
		return -1;
	}
	int index = GetItemCount();
	long id = wxListCtrl::InsertItem(index,"");
	SetItem(id, 0, wxString::Format("%d", id));
	SetItem(id, 1, label);
	SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
	mSelectedItem.push_back(0);
	mFilesStateList.insert({ label,id });
	mExector->commit(CalFileHash, label, this);
	index = GetItemCount();
	mCalculatingFilesCount++;
	return index;
}

void FileHashInfoList::OnDoubleClick(wxListEvent& event)
{
	long row = event.GetIndex();
	wxString title = "文件信息";
	GetCellContentsString(row, 1, title);
	wxString msg;
	wxString crc32;
	wxString md5;
	if (GetCellContentsString(row, 2, crc32))
	{
		msg += "crc32: " + crc32 + "\n";
	}
	if (GetCellContentsString(row, 3, md5))
	{
		msg += "md5:   " + md5;
	}
	wxMessageDialog message(this, msg, title, wxCANCEL | wxYES_NO | wxICON_INFORMATION);
	message.SetYesNoCancelLabels("复制MD5", "复制CRC32", "OK");
	int result = message.ShowModal();
	if (result == wxID_YES)
	{
		if (mClipBoard->Open())
		{
			mClipBoard->SetData(new wxTextDataObject(md5));
			mClipBoard->Close();
		}
	}
	else if (result == wxID_NO)
	{
		if (mClipBoard->Open())
		{
			mClipBoard->SetData(new wxTextDataObject(crc32));
			mClipBoard->Close();
		}
	}
}

void FileHashInfoList::OnItemSelected(wxListEvent& event)
{
	int id = event.GetIndex();
	mSelectedItem[event.GetIndex()] = 1;
	//if (mCallBack != nullptr)
	//{
	//	mCallBack->onMessage(std::to_string(event.GetIndex()));
	//}
}

void FileHashInfoList::OnItemDeSelected(wxListEvent& event)
{
	int id = event.GetIndex();
	mSelectedItem[event.GetIndex()] = 0;
}

void FileHashInfoList::OnItemRightClick(wxListEvent& event)
{
	wxPoint pos = event.GetPoint();
	wxMenu menu;
	//menu.Append(wxID_ANY, wxString::Format("Menu for item %ld", msg));
	menu.Append(ID_COPY_FILE_PATH, "&复制选中项文件路径");
	menu.Append(ID_COPY_CRC32, "&复制选中项CRC32");
	menu.Append(ID_COPY_MD5, "&复制选中项MD5");
	menu.Append(ID_COPY_FILE_PATH_MD5, "&复制选中项文件路径和MD5");
	menu.Append(ID_COPY_FILE_PATH_CRC32_MD5, "&复制选中项文件路径，CRC32和MD5");

	PopupMenu(&menu, pos.x, pos.y);
}

bool FileHashInfoList::GetCellContentsString(long row_number, int column, wxString& cell_contents_string) const
{
	wxListItem row_info;

	// Set what row it is (m_itemId is a member of the regular wxListCtrl class)
	row_info.m_itemId = row_number;
	// Set what column of that row we want to query for information.
	row_info.m_col = column;
	// Set text mask
	row_info.m_mask = wxLIST_MASK_TEXT;

	// Get the info and store it in row_info variable.
	if (GetItem(row_info))
	{
		// Extract the text out of that cell
		cell_contents_string = row_info.m_text;
		return true;
	}

	return false;
}

bool FileHashInfoList::IsExistSuchFile(const wxString& filepath)
{
	std::unordered_map<wxString, long>::const_iterator iter = mFilesStateList.find(filepath);
	return iter != mFilesStateList.end();
}

void FileHashInfoList::OnStartCalculate(const wxString& filepath)
{
	std::unique_lock<std::recursive_mutex> _l(mFileListLock);
	std::unordered_map<wxString, long>::const_iterator iter = mFilesStateList.find(filepath);
	if (iter != mFilesStateList.end())
	{
		long id = iter->second;
		SetItem(id, 4, wxString::Format("0%%"));
	}
	if (mCalculatingFilesCount > 0)
	{
		mCallBack->onMessage(wxString::Format("还有%zu个文件在计算中", mCalculatingFilesCount));
	}
}

void FileHashInfoList::OnProgress(const wxString& filepath, int progress)
{
	std::unique_lock<std::recursive_mutex> _l(mFileListLock);
	std::unordered_map<wxString, long>::const_iterator iter = mFilesStateList.find(filepath);
	if (iter != mFilesStateList.end())
	{
		long id = iter->second;
		SetItem(id, 4, wxString::Format("%d%%", progress));
	}
}

void FileHashInfoList::OnStopCalculate(const wxString& filepath, const std::string& crc32, const std::string& md5)
{
	std::unique_lock<std::recursive_mutex> _l(mFileListLock);
	std::unordered_map<wxString, long>::const_iterator iter = mFilesStateList.find(filepath);
	if (iter != mFilesStateList.end())
	{
		long id = iter->second;
		SetItem(id, 2, crc32);
		SetItem(id, 3, md5);
		SetItem(id, 4, "100%");
	}
	else
	{
		wxMessageBox(filepath,"没找到目标文件", wxOK | wxICON_INFORMATION);
	}
	mCalculatingFilesCount--;
	if (mCalculatingFilesCount > 0)
	{
		mCallBack->onMessage(wxString::Format("还有%zu个文件在计算中", mCalculatingFilesCount));
	}
	else
	{
		mCallBack->onMessage(wxString::Format("全部文件都已经计算完成"));
	}
}

void FileHashInfoList::CopySelectedFilePath(wxCommandEvent& event)
{
	CopySelectedFileInfo(true, false, false);
}

void FileHashInfoList::CopySelectedFileCrc32(wxCommandEvent& event)
{
	CopySelectedFileInfo(false, true, false);
}

void FileHashInfoList::CopySelectedFileMd5(wxCommandEvent& event)
{
	CopySelectedFileInfo(false, false, true);
}

void FileHashInfoList::CopySelectedFilePathAndMd5(wxCommandEvent& event)
{
	CopySelectedFileInfo(true, false, true);
}

void FileHashInfoList::CopySelectedFileInfo(wxCommandEvent& event)
{
	CopySelectedFileInfo(true, true, true);
}

void FileHashInfoList::CopySelectedAllFileInfo(wxCommandEvent& event)
{
	std::unique_lock<std::recursive_mutex> _ll(mFileListLock);
	int itemsize = GetItemCount();
	
	if (itemsize > 0)
	{
		wxString data;
		for (int i = 0; i < itemsize; i++)
		{
			wxString msg;
			bool needspace = false;
			if (GetCellContentsString(i, 1, msg))
			{
				data += msg;
				needspace = true;
			}
			if (GetCellContentsString(i, 2, msg))
			{
				data += (needspace ? "  " : "") + msg;
				needspace = true;
			}

			if (GetCellContentsString(i, 3, msg))
			{
				data += (needspace ? "  " : "") + msg;
				needspace = true;
			}
			data += "\n";
		}
		if (mClipBoard->Open())
		{
			mClipBoard->SetData(new wxTextDataObject(data));
			mClipBoard->Close();
			//mNotify = new wxNotificationMessage("复制成功", "", mParent, wxICON_INFORMATION);
			//mNotify->Show(-1);
		}
		else
		{
			wxMessageBox("打开剪切板失败", "警告", wxOK | wxICON_WARNING);
		}
	}
	else
	{
		wxMessageBox("现在没有添加过任何文件", "警告", wxOK | wxICON_WARNING);
	}
}

void FileHashInfoList::ClearAllItem(wxCommandEvent& event)
{
	std::unique_lock<std::recursive_mutex> _ll(mFileListLock);
	size_t size = mSelectedItem.size();
	for (size_t i = 0; i < size; i++)
	{
		mSelectedItem[i] = 0;
	}
	mFilesStateList.clear();
	if (DeleteAllItems())
	{
		// 清除成功
	}
	else
	{
		wxMessageBox("清除失败", "警告", wxOK | wxICON_WARNING);
	}
}

void FileHashInfoList::SaveAllFileInfo(FILE* file)
{
	if (file)
	{
		std::unique_lock<std::recursive_mutex> _ll(mFileListLock);
		int itemsize = GetItemCount();

		if (itemsize > 0)
		{
			
			for (int i = 0; i < itemsize; i++)
			{
				wxString data;
				wxString msg;
				bool needspace = false;
				if (GetCellContentsString(i, 1, msg))
				{
					data += msg;
					needspace = true;
				}
				if (GetCellContentsString(i, 2, msg))
				{
					data += (needspace ? "  " : "") + msg;
					needspace = true;
				}

				if (GetCellContentsString(i, 3, msg))
				{
					data += (needspace ? "  " : "") + msg;
					needspace = true;
				}
				data += "\n";
				fwrite(data.c_str(), 1, strlen(data.c_str()), file);
			}
		}
		else
		{
			wxMessageBox("现在没有添加过任何文件", "警告", wxOK | wxICON_WARNING);
		}
	}
}

bool FileHashInfoList::IsCalculatFinished()
{
	std::unique_lock<std::recursive_mutex> _ll(mFileListLock);
	return mCalculatingFilesCount == 0;
}

void FileHashInfoList::CopySelectedFileInfo(bool copyFilePath = false, bool copyFileCrc32 = false, bool copyFileMd5 =false)
{
	std::unique_lock<std::recursive_mutex> _ll(mFileListLock);
	size_t size = mSelectedItem.size();
	size_t selectedItemCount = 0;
	size_t failedCount = 0;
	wxString data;
	for (size_t i = 0; i < size; i++)
	{
		bool needspace = false;
		if (mSelectedItem[i] == 1)
		{
			if (selectedItemCount > 0)
			{
				data += "\n";
			}
			selectedItemCount++;
			wxString msg;
			if (copyFilePath)
			{
				if (GetCellContentsString(i, 1, msg))
				{
					data += msg;
					needspace = true;
				}
				else
				{
					failedCount++;
				}
			}
			if (copyFileCrc32)
			{
				if (GetCellContentsString(i, 2, msg))
				{
					data += (needspace ? "  " : "") + msg;
					needspace = true;
				}
				else
				{
					failedCount++;
				}
			}
			if (copyFileMd5)
			{
				if (GetCellContentsString(i, 3, msg))
				{
					data += (needspace ? "  " : "") + msg;
				}
				else
				{
					failedCount++;
				}
			}

			//else
			//{
			//	failedCount++;
			//	wxMessageBox(wxString::Format("获取单元格内容失败啦 %zu", i), "警告", wxOK | wxICON_WARNING);
			//}
		}
	}
	if (selectedItemCount == 0)
	{
		wxMessageBox("没有选中任何条目", "警告", wxOK | wxICON_WARNING);
		return;
	}
	if (failedCount == 0)
	{
		if (mClipBoard->Open())
		{
			mClipBoard->SetData(new wxTextDataObject(data));
			mClipBoard->Close();
			//mNotify = new wxNotificationMessage("复制成功", "", mParent, wxICON_INFORMATION);
			//mNotify->Show(-1);
		}
		else
		{
			wxMessageBox("打开剪切板失败", "警告", wxOK | wxICON_WARNING);
		}
	}
	else
	{
		wxMessageBox("获取单元格内容失败", "警告", wxOK | wxICON_WARNING);
	}
}
