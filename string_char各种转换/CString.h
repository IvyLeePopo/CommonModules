/*
 * @Description: CString学习
				CString 是 MFC（Microsoft Foundation Classes）库中的一种字符串类。
				它是一个用于处理字符数据的类，提供了许多方便的方法和操作符来操作字符串。
 * @Author: Ivy
 * @Date: 2023-06-29 09:34:43
 * @LastEditTime: 2023-06-29 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

// 打印十六进制的字符
void printfCString();

//读卡器发出来的指令是 char* 
//1、转化为十六进制的字符，打印出来
void printf_char2HexString();

//CString转char*
const char* szStr = strLog.GetString();

//char*转string
std::string cmd_data(szStr);

//数据格式之间的转换：
//1、QByteArray转vector
void QByteArray2vector();

//2、vector转QByteArray
void vector2QByteArray();





