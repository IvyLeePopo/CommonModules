/*
 * @Description: 通讯协议相关的接口
 * @Author: Ivy
 * demo :
 * @Date: 2022-05-08 09:34:43
 * @LastEditTime: 2022-05-08 13:39:52
 * @LastEditors: XTZJ-2022OQEHLZ
 */

/** 基础帧数据结构定义
	设备通讯之间的的数据帧格式分两种，一种是有正常数据，一种是空数据域。
	正常：STX	LEN	SN	TOPCMD	SUBCMD	DATA	VERIFY	ETX
	空数据：STX	LEN	SN	TOPCMD	SUBCMD	VERIFY	ETX
*/

#pragma once 
#include <QByteArray>

namespace ProtocolParse
{
	// 收到原始数据，要做一系列的操作
	void ParseProtocolData(std::vector<std::string>& c0_queue, std::vector<unsigned char>& real_data);

	// 打包数据，再发送
	bool packBytes(eSubCmdType_t subCmdType, const string& jsonData, u8* pcucDstData, u32& nDstLen);

	// CRC校验
	bool crc16(const u8* byte, int len, u8* cHi, u8* cLo);
	unsigned short crc16(const std::vector<unsigned char>::const_iterator& it_first, const std::vector<unsigned char>::const_iterator& it_second);


	// 转义
	bool EscapeComProtocol(const std::vector<unsigned char>& src, std::vector<unsigned char>&dst);
	bool UnEscapeComProtocol(const std::vector<unsigned char>& src, std::vector<unsigned char>&dst);

};

class ProtocolParse
{
public:
	ProtocolParse();

public:
    //数据处理
    static bool checkPackage(const std::vector<uint8>& vecData);     //CRC校验判断
    static vector<uint8> crc16(vector<uint8> &byte, uint32 len);     //CRC校验
    static vector<uint8> escapeComProtocol(vector<uint8> &lpData, uint32 len, uint32 bSendState);   //转译，bSendState：1发送，0接收
    
	//编码格式转换																								  
    static vector<uint8> vectorUTF8ToGBK(char *msg, int len);
    static QByteArray bytesUTF8ToGBK(char *msg, int len);
    static vector<uint8> vectorGBKToUTF8(char *msg, int len);
    static QByteArray bytesGBKToUTF8(char *msg, int len);
  
	//packet打包
    static vector<uint8> packBytes(DevFunctionType1 actionType, uint8 objectType, vector<uint8>& objectData);
    static vector<uint8> packBytes(uint8 actionType, uint8 objectType, char *msg, int len);
  
	//unpacket解包
    static vector<vector<uint8>> unPackBytes(vector<uint8>& data);
};