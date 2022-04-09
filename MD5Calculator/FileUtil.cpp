#define _CRT_SECURE_NO_WARNINGS

#include "FileUtil.h"
#include "crc32.h"
#include <openssl/md5.h>
#include <sys/stat.h>
#include <ctime>
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

void CalFileHash(wxString filename, FileUtil* callback)
{
	callback->OnStartCalculate(filename);
	std::string crc32val;
	std::string md5val;
	struct _stat64 info;
	_stat64(filename.c_str(), &info);
	FILE* file = fopen(filename.c_str(), "rb");
	if (file && info.st_size > 0)
	{
		int buffersize = MapBlockSize(info.st_size);
		unsigned char* buffer = (unsigned char*)malloc(buffersize);

		if (buffer)
		{
			uint32_t crcvalue = 0;
			MD5_CTX c;
			MD5_Init(&c);
			unsigned char md5[16] = { 0 };
			long long totalReadedSize = 0;
			int readsize = (int)fread(buffer, 1, buffersize, file);
			long before = std::clock();
			while (readsize > 0)
			{
				totalReadedSize += readsize;
				crcvalue = calculate_crc32(crcvalue, buffer, (unsigned int)readsize);
				MD5_Update(&c, buffer, readsize);
				long now = std::clock();
				if ((now - before) > 100)
				{
					// 可能会溢出
					int progress = totalReadedSize * 100 / info.st_size;
					before = now;
					callback->OnProgress(filename,progress);
				}
				readsize = (int)fread(buffer, 1, buffersize, file);
			}
			free(buffer);
			MD5_Final(md5, &c);
			crc32val = wxString::Format("%08X", crcvalue).ToStdString();
			md5val = wxString::Format("%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
				md5[0], md5[1], md5[2], md5[3], md5[4], md5[5], md5[6], md5[7], md5[8], md5[9], md5[10], md5[11], md5[12], md5[13], md5[14], md5[15]).ToStdString();
		}
		else
		{
			// out of memory
		}
		fclose(file);
		
	}
	else
	{
		if (info.st_size == 0)
		{
			MD5_CTX c;
			MD5_Init(&c);
			std::string nullstr = "";
			MD5_Update(&c, nullstr.c_str(), nullstr.size());
			unsigned char md5[16] = { 0 };
			MD5_Final(md5, &c);
			md5val = wxString::Format("%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
				md5[0], md5[1], md5[2], md5[3], md5[4], md5[5], md5[6], md5[7], md5[8], md5[9], md5[10], md5[11], md5[12], md5[13], md5[14], md5[15]).ToStdString();
		}
		if (!file)
		{
			crc32val = "failed";
		}
	}
	callback->OnStopCalculate(filename, crc32val, md5val);
}
