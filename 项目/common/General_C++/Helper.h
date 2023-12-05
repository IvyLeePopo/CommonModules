#pragma once
#include "SingleTon.h"

#ifndef nullptr
#define nullptr NULL
#endif
typedef unsigned long ULONG;

class CHelper : public SingleTon<CHelper>
{
	friend class SingleTon<CHelper>;

public:
	CHelper(void);
	virtual ~CHelper(void);

	struct tagFileInfo
	{
		ULONG uLen;
		int nCreateTime;
		int nModifyTime;
		string strFileName;
	};

public:
	// 字符串的拼接
	string format(const char* pFmt, ...);

	// 数字转字符串
	string i2a(int nValue);

	// 浮点转字符串 nPoint 指的是小数点后几位
	string f2a(float fValue, int nPoint = 4);

	// 获取时间戳,精确到秒，int类型
	__int64 getCurrentTimeStamp(bool bMSecond = false); 

	// 获取字符串的时间，精确到字符串，返回字符串如下：2022-09-03 10:09:13.958 \ 2022-09-03 10:09:13,   
	// 如bUseSeparator为false，则格式为20220903100913958 \ 20220903100913
	string getCurrentTimeString(bool bMSecond = true, bool bUseSeparator = true);

	//	获取字符串的时间，精确到字符串，返回字符串如下：2022-09-03星期四 10:09:13.958 \ 2022-09-03星期四 10:09:13
	string getCurrentTimeString_DayofWeek(bool bMSecond = true);

	// 获取当前的日期，直接返回int类型，返回的格式如下：20221102
	int getCurrentDate();

	// 是否为匹配的时间,默认0点0时0分
	bool isEqualTime(int nHour = 0, int nMin = 0, int nSec = 0);

	// 获取md5加密
	string md5(const char* pszBuf);

	// 获取进程运行的路径
	string getProcessPath();

	// 是否存在文件或者目录
	bool isExistFile(string strFile, bool bIsDir = false);

	// 获取指定目录下的所有指定格式文件,返回的列表将按照创建时间排序，最早的在最前面
	bool getDirFiles(string strDir, vector<CHelper::tagFileInfo>& vecFiles,string strSuffixName);

	// 保存文件内容
	/*
		strMode词典如下:
		r	以只读方式打开文件，该文件必须存在。
		r+	以读/写方式打开文件，该文件必须存在。
		rb+	以读/写方式打开一个二进制文件，只允许读/写数据。
		rt+	以读/写方式打开一个文本文件，允许读和写。
		w	打开只写文件，若文件存在则文件长度清为零，即该文件内容会消失；若文件不存在则创建该文件。
		w+	打开可读/写文件，若文件存在则文件长度清为零，即该文件内容会消失；若文件不存在则创建该文件。
		a	以附加的方式打开只写文件。若文件不存在，则会创建该文件；如果文件存在，则写入的数据会被加到文件尾后，即文件原先的内容会被保留（EOF 符保留）。
		a+	以附加方式打开可读/写的文件。若文件不存在，则会创建该文件，如果文件存在，则写入的数据会被加到文件尾后，即文件原先的内容会被保留（EOF符不保留）。
		wb	以只写方式打开或新建一个二进制文件，只允许写数据。
		wb+	以读/写方式打开或新建一个二进制文件，允许读和写。
		wt+	以读/写方式打开或新建一个文本文件，允许读和写。
		at+	以读/写方式打开一个文本文件，允许读或在文本末追加数据。
		ab+	以读/写方式打开一个二进制文件，允许读或在文件末追加数据。
	*/
	bool setFileContent(string strFile, string& strContent, string strMode = "w+");

	// 获取文件内容
	bool getFileContent(string strFile, string& strContent, bool bHex = false);
	
	// 保存二进制文件内容
	bool setFileBinary(string strFile, const char* pszData, int nSize);

	// 获取文件内容，二进制的方式,谨记，用过本函数需要释放pszData！！！！！！！！
	bool getFileBinary(string strFile, char** pszData, int& nSize);

	// 新建文件或者文件夹
	bool createNewFileOrDir(string strFile, bool bIsDir = false);

	// 创建随机32位UID
	string generate32UID();

	// sha256进行加密
	string encryptSha256(const char* pszData, int nSize);

	// base64加密 返回加密后的长度
	int encodeBase64(unsigned char * pszInput, int nLength, char * pszOutput);

	// base64解密 返回解密后的长度
	int decodeBase64(unsigned char * pszInput, int nLength, char * pszOutput);

	// 获取模块的路径，调用这后面自己加"\"
	string getModulePath();

	// crc16算法
	unsigned short crc16(const char* pszData, int nSize);
	unsigned short crc16(const std::vector<unsigned char>::const_iterator& it_first,
		const std::vector<unsigned char>::const_iterator& it_second);

	// 判断ip是否合法
	bool isVaildIp(const char* pszIp);

	// 截取从websocket传输来的经过base64加密的图片
	string splitImageFromBase64(const string& strData, string& strType);

	// 字符串截取 示例 ("1,2,3,4", ",", vector)
	void splitString(string& strSource, string strReps, vector<string>& vecString);

	// utf8->gbk
	string utf8ToGbk(const char *pszSrc);

	// gbk->utf8
	string gbk2Utf8(string& strData);

	// 根据 起始位+长度 替换字符
	string eraseString(const char* pszSource, int nStartPos, int nLength);

	// 根据 字符内容 替换字符
	string replaceString(const char* pszSource, const char* pszOldKey, const char* pszNewKey);

	// windows独有的多个独立动态库之间数据交互，使用sharedfilemap，直接写入即可(Linux下可使用管道替代即可)
	bool write2SharedFileMap(const char* pszDataKey, const char* pszData, int nSize);

	// 读取sharedfilemap数据
	bool readSharedFileMap(const char* pszDataKey, string& strData);

	// 清除共享内存的数据
	bool clearSharedFileMap(const char* pszDataKey);

	// 从例“192.168.0.11,8000,admin,password”分离出来各个数据
	bool splitIpAndPort(const char* pszData, string& strIp, int& nPort, string& strUser, string& strPasswd);

	// 获取dll、exe的版本、公司等信息
	bool getDllExeVersion(const string& strName, string& strCompany, string& strVersion, string& strDescrip);

	// 获取特定范围的随机数字
	int getRandValue();

	// 将16进制转换成字符串
	string hex2String(const char* pszBuf, int nLen);

	// 获取文件的长度
	long getFileSize(const char* pszFileName);

	// 删除文件
	bool deleteFile(const char* pszFileName);

	//获取bmp图片的二进制数据
	bool jpeg2Bmp(const char* pszSrc, int nSrcLength, unsigned char** pszDst, unsigned int& nDstLength);

	//获取bmp图片的二进制数据
	bool Bmp2Binarization(const string& bmpFilePath, unsigned char** pszDst, unsigned int& nDstLength);

};

#define Helper CHelper::getInstance()
