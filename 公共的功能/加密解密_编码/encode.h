/*
 * @Description: 常用的一些加密解密、以及转码的方法
 * @Author: Ivy
 * @Date: 2022-05-11 09:34:43
 * @LastEditTime: 2022-05-11 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once


namespace base64
{
	// 1、base64加密 返回加密后的长度
	int encodeBase64(unsigned char * pszInput, int nLength, char * pszOutput);

	// 2、base64解密 返回解密后的长度
	int decodeBase64(unsigned char * pszInput, int nLength, char * pszOutput);

	// 3、base64 加密：string to base64
	string string2Base64_qt(const char* pszData, unsigned int nLen);

	// 4、base64 to string
	string base64ToString_qt(string strData);
}


namespace sha_md5_crc16
{
	// 1、sha256进行加密
	string encryptSha256(const char* pszData, int nSize);

	// 2、crc16算法
	/*
	CRC校验是一种错误检测技术，它使用一种称为循环冗余校验（CRC）算法的数学函数来检测数据传输中的错误
	*/
	unsigned short crc16(const char* pszData, int nSize);
	unsigned short crc16(const std::vector<unsigned char>::const_iterator& it_first,
		const std::vector<unsigned char>::const_iterator& it_second);

	// crc校验：另一种校验
	uint16_t crc16_2(uint8_t* puchMsg, uint32_t usDataLen);

	// 3、获取md5加密
	/**
	使用openSSL开源库中的 MD5 函数
	OpenSSL是一个开源的软件库，提供了一系列的加密和解密功能，包括SSL和TLS协议的实现。
	它支持常见的加密算法，如AES、DES、RSA等，可以用于安全地传输数据、保护用户的隐私信息等。
	OpenSSL不仅可以用于服务器端和客户端的安全通信，还可以用于加密文件、数字签名等方面。
	*/
	string md5_OpenSSL(const string& input);
}

namespace codeTrans
{
	// 1、utf8->gbk
	string utf8ToGbk(const char *pszSrc);

	// 2、gbk->utf8
	string gbk2Utf8(string& strData);

	// 3、gbk->utf8
	string G2U_qt(string strGBK);

	// 4、国标编码 转 utf8编码
	QString GBK_2_UTF8_qt(const char* p);

	// 5、utf8编码 转 国标编码
	QString UTF8_2_GBK_qt(const char* p);
}
