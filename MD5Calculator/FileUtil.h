#pragma once
#include <string>
#include <wx/string.h>
enum FileCalState
{
	NOT_START_CALCULATE,
	STARTED,
	CALCULATING,
	CALCULATED
};
struct FileState {
	long id;
	std::string crc32;
	std::string md5;
	FileCalState state;
};
class FileUtil
{
public:
	virtual void OnStartCalculate(const wxString& filepath) = 0;
	virtual void OnProgress(const wxString& filepath,int progress) = 0;
	virtual void OnStopCalculate(const wxString& filepath,const std::string& crc32,const std::string& md5) = 0;
};

void CalFileHash(wxString filename,FileUtil* callback);