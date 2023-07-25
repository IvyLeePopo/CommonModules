#include "StringChar.h"
#include <stdio.h>
#include <stdlib.h>

void string2Char()
{
	//string转char*
	string str = "hello";
	char* ch = str.c_str;

	//char*转string 
	//C
	char* ch = "hello";
	std::string str(ch);
	//C++
	char* ch = "hello";
	std::string str = ch;
}

void string2UnsignedChar()
{
	//string转unsigned char*
	string str = "hello";
	unsigned char *uc = (unsigned char *)str.c_str();//强转，可以参考下面安全转换

	//unsigned char*转string
	string str = std::string(char*)ch;
}

void string2QString()
{
}

/**
	char* 转unsigned char* 的安全转换方法如下：
	1. 首先，我们需要定义一个指向 char 类型的指针和一个指向 unsigned char 类型的指针。
		char* charPtr;
		unsigned char* ucharPtr;
	2. 然后，我们需要将 char 类型的指针强制转换为 void* 类型的指针。
		void* voidPtr = static_cast<void*>(charPtr);
	3. 接下来，我们将 void* 类型的指针强制转换为 unsigned char* 类型的指针。
		ucharPtr = static_cast<unsigned char*>(voidPtr);
	4. 最后，我们可以使用 ucharPtr 指针访问 unsigned char 类型的数据，而不会出现任何问题。

	这种安全转换方法可以确保我们在访问 unsigned char 数据时不会出现任何问题，
	因为我们已经将 char* 指针转换为 unsigned char* 指针，而不是直接访问 char* 数据。
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
	//int转string
	int num = 123;
	std::string str = std::to_string(num);

	//double转stirng
	double d = 3.14159;
	std::string str = std::to_string(d);

/**********************************************************************************/

	//string转int
	char str[] = "123";
	int num = atoi(str);  
	printf("The integer is %d\n", num);

	//string转double
	char str[] = "3.14";
	double num = atof(str);
	printf("num = %lf\n", num);
}


/////////////////////////////////////////////////////////////////////////////////////////
////函数名称:fnHexToBin
////函数功能:将16进制的字符串转换成2进制的数字
////输入参数:IN PBYTE	pHex		-- 需要转换的16进制字符串
////		 OUT PBYTE	pbin		-- 输出的2进制数字数组
////		 IN DWORD	dwHex_len	-- 需要转换的16进制数据长度,必须为2的倍数
////输出参数:无
////编写时间:2008-11-08
////编写人员:ROCY
////修改时间:
////修改人员:
////函数版本:1.0.0.1
////备注说明:所输入的16进制字符串长度必须是2的倍数
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
////函数名称:BinToHex
////函数功能:将2进制的数字转换成16进制的字符串
////输入参数:IN PBYTE	pBin		-- 需要转换的2进制数字数组
////		 IN DWORD	dwHex_len	-- 需要转换的2进制数字数组长度
////输出参数:CString	非空 -- 转换成功后输出的16进制字符串
////		 CString	NULL -- 转换失败
////编写时间:2008-11-08
////编写人员:ROCY
////修改时间:
////修改人员:
////函数版本:1.0.0.1
////备注说明:如果调用成功后,不需要调用者释放内存
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