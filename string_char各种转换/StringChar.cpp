#include "StringChar.h"
#include <stdio.h>
#include <stdlib.h>

void string2Char()
{
	//stringתchar*
	string str = "hello";
	char* ch = str.c_str;

	//char*תstring 
	//C
	char* ch = "hello";
	std::string str(ch);
	//C++
	char* ch = "hello";
	std::string str = ch;
}

void string2UnsignedChar()
{
	//stringתunsigned char*
	string str = "hello";
	unsigned char *uc = (unsigned char *)str.c_str();//ǿת�����Բο����氲ȫת��

	//unsigned char*תstring
	string str = std::string(char*)ch;
}

void string2QString()
{
}

/**
	char* תunsigned char* �İ�ȫת���������£�
	1. ���ȣ�������Ҫ����һ��ָ�� char ���͵�ָ���һ��ָ�� unsigned char ���͵�ָ�롣
		char* charPtr;
		unsigned char* ucharPtr;
	2. Ȼ��������Ҫ�� char ���͵�ָ��ǿ��ת��Ϊ void* ���͵�ָ�롣
		void* voidPtr = static_cast<void*>(charPtr);
	3. �����������ǽ� void* ���͵�ָ��ǿ��ת��Ϊ unsigned char* ���͵�ָ�롣
		ucharPtr = static_cast<unsigned char*>(voidPtr);
	4. ������ǿ���ʹ�� ucharPtr ָ����� unsigned char ���͵����ݣ�����������κ����⡣

	���ְ�ȫת����������ȷ�������ڷ��� unsigned char ����ʱ��������κ����⣬
	��Ϊ�����Ѿ��� char* ָ��ת��Ϊ unsigned char* ָ�룬������ֱ�ӷ��� char* ���ݡ�
*/
void char2unsignedChar()
{
	char* charPtr;
	unsigned char* ucharPtr;
	void* voidPtr = static_cast<void*>(charPtr);
	ucharPtr = static_cast<unsigned char*>(voidPtr);
}

void int2String()
{
	//intתstring
	int num = 123;
	std::string str = std::to_string(num);

	//doubleתstirng
	double d = 3.14159;
	std::string str = std::to_string(d);

/**********************************************************************************/

	//stringתint
	char str[] = "123";
	int num = atoi(str);  
	printf("The integer is %d\n", num);

	//stringתdouble
	char str[] = "3.14";
	double num = atof(str);
	printf("num = %lf\n", num);
}


/////////////////////////////////////////////////////////////////////////////////////////
////��������:fnHexToBin
////��������:��16���Ƶ��ַ���ת����2���Ƶ�����
////�������:IN PBYTE	pHex		-- ��Ҫת����16�����ַ���
////		 OUT PBYTE	pbin		-- �����2������������
////		 IN DWORD	dwHex_len	-- ��Ҫת����16�������ݳ���,����Ϊ2�ı���
////�������:��
////��дʱ��:2008-11-08
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////�����汾:1.0.0.1
////��ע˵��:�������16�����ַ������ȱ�����2�ı���
//////////////////////////////////////////////////////////////////////////////////////////
void Hex2Bin(IN LPTSTR lpHex, OUT PBYTE pBin, IN DWORD dwHex_len)
{
	if (dwHex_len % 2)
		return;

	CString strTemp(lpHex);
	strTemp.MakeUpper();

	USES_CONVERSION;
	for (int i = 0; i < strTemp.GetLength() / 2; i++)
	{
		BYTE a = (BYTE)(strTemp.GetAt(i * 2) >= 'A') ? (BYTE)(strTemp.GetAt(i * 2) - 'A' + 10) : (BYTE)(strTemp.GetAt(i * 2) - '0');
		BYTE b = (BYTE)(strTemp.GetAt(i * 2 + 1) >= 'A') ? (BYTE)(strTemp.GetAt(i * 2 + 1) - 'A' + 10) : (BYTE)(strTemp.GetAt(i * 2 + 1) - '0');
		pBin[i] = (BYTE)(a * 0x10 + b);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:BinToHex
////��������:��2���Ƶ�����ת����16���Ƶ��ַ���
////�������:IN PBYTE	pBin		-- ��Ҫת����2������������
////		 IN DWORD	dwHex_len	-- ��Ҫת����2�����������鳤��
////�������:CString	�ǿ� -- ת���ɹ��������16�����ַ���
////		 CString	NULL -- ת��ʧ��
////��дʱ��:2008-11-08
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////�����汾:1.0.0.1
////��ע˵��:������óɹ���,����Ҫ�������ͷ��ڴ�
//////////////////////////////////////////////////////////////////////////////////////////
CString Bin2Hex(IN PBYTE pBin, IN DWORD dwBin_Len)
{
	CString strHex;
	CString strTemp;
	if (NULL != pBin)
	{
		for (DWORD i = 0; i < dwBin_Len; ++i)
		{
			strTemp.Format(_T("%02X"), pBin[i]);
			strHex += strTemp;
		}
	}
	return strHex;
}