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
	// �ַ�����ƴ��
	string format(const char* pFmt, ...);

	// ����ת�ַ���
	string i2a(int nValue);

	// ����ת�ַ��� nPoint ָ����С�����λ
	string f2a(float fValue, int nPoint = 4);

	// ��ȡʱ���,��ȷ���룬int����
	__int64 getCurrentTimeStamp(bool bMSecond = false); 

	// ��ȡ�ַ�����ʱ�䣬��ȷ���ַ����������ַ������£�2022-09-03 10:09:13.958 \ 2022-09-03 10:09:13,   
	// ��bUseSeparatorΪfalse�����ʽΪ20220903100913958 \ 20220903100913
	string getCurrentTimeString(bool bMSecond = true, bool bUseSeparator = true);

	//	��ȡ�ַ�����ʱ�䣬��ȷ���ַ����������ַ������£�2022-09-03������ 10:09:13.958 \ 2022-09-03������ 10:09:13
	string getCurrentTimeString_DayofWeek(bool bMSecond = true);

	// ��ȡ��ǰ�����ڣ�ֱ�ӷ���int���ͣ����صĸ�ʽ���£�20221102
	int getCurrentDate();

	// �Ƿ�Ϊƥ���ʱ��,Ĭ��0��0ʱ0��
	bool isEqualTime(int nHour = 0, int nMin = 0, int nSec = 0);

	// ��ȡmd5����
	string md5(const char* pszBuf);

	// ��ȡ�������е�·��
	string getProcessPath();

	// �Ƿ�����ļ�����Ŀ¼
	bool isExistFile(string strFile, bool bIsDir = false);

	// ��ȡָ��Ŀ¼�µ�����ָ����ʽ�ļ�,���ص��б����մ���ʱ���������������ǰ��
	bool getDirFiles(string strDir, vector<CHelper::tagFileInfo>& vecFiles,string strSuffixName);

	// �����ļ�����
	/*
		strMode�ʵ�����:
		r	��ֻ����ʽ���ļ������ļ�������ڡ�
		r+	�Զ�/д��ʽ���ļ������ļ�������ڡ�
		rb+	�Զ�/д��ʽ��һ���������ļ���ֻ�����/д���ݡ�
		rt+	�Զ�/д��ʽ��һ���ı��ļ����������д��
		w	��ֻд�ļ������ļ��������ļ�������Ϊ�㣬�����ļ����ݻ���ʧ�����ļ��������򴴽����ļ���
		w+	�򿪿ɶ�/д�ļ������ļ��������ļ�������Ϊ�㣬�����ļ����ݻ���ʧ�����ļ��������򴴽����ļ���
		a	�Ը��ӵķ�ʽ��ֻд�ļ������ļ������ڣ���ᴴ�����ļ�������ļ����ڣ���д������ݻᱻ�ӵ��ļ�β�󣬼��ļ�ԭ�ȵ����ݻᱻ������EOF ����������
		a+	�Ը��ӷ�ʽ�򿪿ɶ�/д���ļ������ļ������ڣ���ᴴ�����ļ�������ļ����ڣ���д������ݻᱻ�ӵ��ļ�β�󣬼��ļ�ԭ�ȵ����ݻᱻ������EOF������������
		wb	��ֻд��ʽ�򿪻��½�һ���������ļ���ֻ����д���ݡ�
		wb+	�Զ�/д��ʽ�򿪻��½�һ���������ļ����������д��
		wt+	�Զ�/д��ʽ�򿪻��½�һ���ı��ļ����������д��
		at+	�Զ�/д��ʽ��һ���ı��ļ�������������ı�ĩ׷�����ݡ�
		ab+	�Զ�/д��ʽ��һ���������ļ�������������ļ�ĩ׷�����ݡ�
	*/
	bool setFileContent(string strFile, string& strContent, string strMode = "w+");

	// ��ȡ�ļ�����
	bool getFileContent(string strFile, string& strContent, bool bHex = false);
	
	// ����������ļ�����
	bool setFileBinary(string strFile, const char* pszData, int nSize);

	// ��ȡ�ļ����ݣ������Ƶķ�ʽ,���ǣ��ù���������Ҫ�ͷ�pszData����������������
	bool getFileBinary(string strFile, char** pszData, int& nSize);

	// �½��ļ������ļ���
	bool createNewFileOrDir(string strFile, bool bIsDir = false);

	// �������32λUID
	string generate32UID();

	// sha256���м���
	string encryptSha256(const char* pszData, int nSize);

	// base64���� ���ؼ��ܺ�ĳ���
	int encodeBase64(unsigned char * pszInput, int nLength, char * pszOutput);

	// base64���� ���ؽ��ܺ�ĳ���
	int decodeBase64(unsigned char * pszInput, int nLength, char * pszOutput);

	// ��ȡģ���·��������������Լ���"\"
	string getModulePath();

	// crc16�㷨
	unsigned short crc16(const char* pszData, int nSize);
	unsigned short crc16(const std::vector<unsigned char>::const_iterator& it_first,
		const std::vector<unsigned char>::const_iterator& it_second);

	// �ж�ip�Ƿ�Ϸ�
	bool isVaildIp(const char* pszIp);

	// ��ȡ��websocket�������ľ���base64���ܵ�ͼƬ
	string splitImageFromBase64(const string& strData, string& strType);

	// �ַ�����ȡ ʾ�� ("1,2,3,4", ",", vector)
	void splitString(string& strSource, string strReps, vector<string>& vecString);

	// utf8->gbk
	string utf8ToGbk(const char *pszSrc);

	// gbk->utf8
	string gbk2Utf8(string& strData);

	// ���� ��ʼλ+���� �滻�ַ�
	string eraseString(const char* pszSource, int nStartPos, int nLength);

	// ���� �ַ����� �滻�ַ�
	string replaceString(const char* pszSource, const char* pszOldKey, const char* pszNewKey);

	// windows���еĶ��������̬��֮�����ݽ�����ʹ��sharedfilemap��ֱ��д�뼴��(Linux�¿�ʹ�ùܵ��������)
	bool write2SharedFileMap(const char* pszDataKey, const char* pszData, int nSize);

	// ��ȡsharedfilemap����
	bool readSharedFileMap(const char* pszDataKey, string& strData);

	// ��������ڴ������
	bool clearSharedFileMap(const char* pszDataKey);

	// ������192.168.0.11,8000,admin,password�����������������
	bool splitIpAndPort(const char* pszData, string& strIp, int& nPort, string& strUser, string& strPasswd);

	// ��ȡdll��exe�İ汾����˾����Ϣ
	bool getDllExeVersion(const string& strName, string& strCompany, string& strVersion, string& strDescrip);

	// ��ȡ�ض���Χ���������
	int getRandValue();

	// ��16����ת�����ַ���
	string hex2String(const char* pszBuf, int nLen);

	// ��ȡ�ļ��ĳ���
	long getFileSize(const char* pszFileName);

	// ɾ���ļ�
	bool deleteFile(const char* pszFileName);

	//��ȡbmpͼƬ�Ķ���������
	bool jpeg2Bmp(const char* pszSrc, int nSrcLength, unsigned char** pszDst, unsigned int& nDstLength);

	//��ȡbmpͼƬ�Ķ���������
	bool Bmp2Binarization(const string& bmpFilePath, unsigned char** pszDst, unsigned int& nDstLength);

};

#define Helper CHelper::getInstance()
