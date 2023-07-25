/*
 * @Description: string、char、int等数据转换
 * @Author: Ivy
 * @Date: 2022-04-24 09:34:43
 * @LastEditTime: 2023-02-24 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

// 1、string转char*
void string2Char();

// 2、string转unsigned char*
void string2UnsignedChar();

// 3、string转QString
void string2QString();

// 4、char* 转unsigned char*
void char2unsignedChar();

// 5、int、double 转 string
void int2String();

// 6、十六进制的字串转为二进制的字符串
void Hex2Bin(IN LPTSTR lpHex, OUT PBYTE pBin, IN DWORD dwHex_len);

// 7、二进制的字串转为十六进制的字符串
CString Bin2Hex(IN PBYTE pBin, IN DWORD dwBin_Len)


