#include "c_file.h"
#include <windows.h>

bool isExistFile(string strFile, bool bIsDir)
{
	if (strFile.empty())
		return false;

	return !bIsDir ? FALSE != PathFileExistsA(strFile.c_str()) : FALSE != PathIsDirectoryA(strFile.c_str());
}

bool getDirFiles(string strDir, vector<tagFileInfo>& vecFiles, string strSuffixName)
 {
	if (strDir.empty() || !isExistFile(strDir, true))
	{
		return false;
	}

	vecFiles.clear();
	string strPath = strDir + "\\*." + strSuffixName;

	// 文件信息
	struct _finddata_t _fileInfo;

	// 文件句柄
	intptr_t hFile = _findfirst(strPath.c_str(), &_fileInfo);
	if (-1 != hFile)
	{
		do
		{
			string strFileName = strDir + "\\" + string(_fileInfo.name);
			if (isExistFile(strFileName))
			{
				tagFileInfo _tagInfo;
				_tagInfo.nCreateTime = static_cast<int>(_fileInfo.time_create);
				_tagInfo.nModifyTime = static_cast<int>(_fileInfo.time_write);
				_tagInfo.uLen = static_cast<int>(_fileInfo.size);
				_tagInfo.strFileName = strFileName;
				vecFiles.push_back(_tagInfo);
			}
		} while (0 == _findnext(hFile, &_fileInfo));
	}
	return true;
 }

bool setFileContent(string strFile, string & strContent, string strMode)
{
	FILE* pFile = fopen(strFile.c_str(), strMode.c_str());
	if (nullptr == pFile)
	{
		return false;
	}

	fwrite(strContent.c_str(), strContent.size(), 1, pFile);
	fflush(pFile);
	fclose(pFile);
	pFile = nullptr;
	return true;
}

long getFileSize(const char * pszFileName)
{
	if (nullptr == pszFileName)
	{
		return 0L;
	}

	long lSize = 0;
	HANDLE pFile = CreateFileA(pszFileName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,        //打开已存在的文件 
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (pFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(pFile);
		return 0;
	}

	lSize = GetFileSize(pFile, NULL);          //得到文件的大小
	CloseHandle(pFile);
	return lSize;
}

bool deleteFile(const char * pszFileName)
{
	if (nullptr == pszFileName)
	{
		return false;
	}

	if (!isExistFile(pszFileName))
	{
		return true;
	}


	return 0 != DeleteFileA(pszFileName);
}

bool getFileContent(string strFile, string& strContent, bool bHex /*= false*/)
{
	//CFile file;
	//UINT uFlag = bHex ? CFile::typeBinary : CFile::modeReadWrite;
	//if (FALSE == file.Open(CString(strFile.c_str()), uFlag))
	//	return false;

	//int nSize = (int)file.GetLength();
	//char* pszBuf = new char[nSize + 1];
	//memset(pszBuf, 0, nSize + 1);

	//file.Read(pszBuf, nSize);
	//file.Close();

	//strContent = pszBuf;
	//delete [] pszBuf;
	//pszBuf = NULL;

	return true;
}

bool setFileBinary(string strFile, const char* pszData, int nSize)
{
	//CFile file;
	//if(FALSE == file.Open(CString(strFile.c_str()), CFile::modeCreate | CFile::typeBinary | CFile::modeNoTruncate | CFile::modeReadWrite))
	//	return false;

	//file.Write(pszData,(UINT)nSize);
	//file.Close();
	return true;
}

#include <fstream>
bool getFileBinary(string strFile, char** pszData, int& nSize)
{
	if (nullptr != *pszData)
	{
		delete[] * pszData;
		*pszData = nullptr;
		return false;
	}

	HANDLE pFile = CreateFileA(strFile.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,        //打开已存在的文件 
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (pFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(pFile);
		return false;
	}

	nSize = GetFileSize(pFile, NULL);          //得到文件的大小
	*pszData = new char[nSize];
	memset(*pszData, 0, nSize);
	char* pTmpBuf = *pszData;

	DWORD dwBytesRead = 0;
	DWORD dwBytesToRead = nSize;
	do { //循环读文件，确保读出完整的文件    

		ReadFile(pFile, pTmpBuf, dwBytesToRead, &dwBytesRead, NULL);
		if (dwBytesRead == 0)
			break;

		dwBytesToRead -= dwBytesRead;
		pTmpBuf += dwBytesRead;
	} while (dwBytesToRead > 0);

	CloseHandle(pFile);
	return true;
}

bool createNewFileOrDir(string strFile, bool bIsDir /*= false*/)
{
	if (isExistFile(strFile, bIsDir))
		return true;

	if (!bIsDir)
	{
		return INVALID_HANDLE_VALUE != CreateFileA(strFile.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_WRITE | FILE_SHARE_READ,
			NULL,
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,
			0);
	}

	return TRUE == CreateDirectoryA(strFile.c_str(), NULL);
}

bool write2File(string strFilePath, string & strContent)
{
	ofstream out(strFilePath.c_str());
	out << strContent.c_str() << endl;
	out.close();
	return true;
}

bool readFile(string strFilePath, string & strContent)
{
	FILE *pFile = fopen(strFilePath.c_str(), "r");
	if (nullptr == pFile)
		return false;

	fseek(pFile, 0, SEEK_END);
	int nLen = ftell(pFile);
	char* pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	fseek(pFile, 0, SEEK_SET);
	fread(pBuf, nLen, 1, pFile);
	fclose(pFile);

	strContent = pBuf;
	delete[] pBuf;
	pBuf = nullptr;

	return true;;
}

bool linux_file::isExistFile(string strFileName)
{
	return access(strFileName.c_str(), F_OK);
}

bool linux_file::createDir(string strDir)
{
	if (isExistFile(strDir))
		return true;

	// 创建目录，需要循环创建每一层的目录
	vector<string> vecDir;
	splitString(strDir, "/", vecDir);

	string strPath = "/";
	for (size_t nIndex = 0; nIndex < vecDir.size(); nIndex++)
	{
		strPath = strPath + vecDir[nIndex] + "/";
		if (isExistFile(strPath))
			continue;

		mkdir(strPath.c_str(), 07777);
	}
	return  true;
}
