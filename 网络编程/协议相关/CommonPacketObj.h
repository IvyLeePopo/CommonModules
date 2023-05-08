/*
 * @Description: 通讯协议相关的接口
 * @Author: Ivy
 * demo :
 * @Date: 2022-05-08 09:34:43
 * @LastEditTime: 2022-05-08 13:39:52
 * @LastEditors: XTZJ-2022OQEHLZ
 */


/** 基础帧数据结构定义
* 1、协议规格
	设备通讯之间的的数据帧格式分两种，一种是有正常数据域的帧格式，一种是空数据域的帧格式。
	正常数据帧格式：STX	LEN	SN	TOPCMD	SUBCMD	DATA	VERIFY	ETX
	空数据域数据帧格式：STX	LEN	SN	TOPCMD	SUBCMD	VERIFY	ETX

* 2、数据帧定义解析：
	字段			长度(Bytes)				描  述
	STX				1					0xDF，帧开始标志
	LEN				2					数据域DATA长度
	SN				2					本帧编号，见3.2描述
	TOPCMD			1					命令大类
	SUBCMD			1					子命令类型
	DATA			LEN					数据域（不定长）
	VERIFY			2					数据校验码，2个字节，见3.3校验码描述
	ETX				1					0xFD，帧结束标志
*3、帧编号（可以填默认值0x00 0x00）
	SN为一段时间内数据帧唯一标示；由发送方生成，从0x 0000 开始自增，到0xFFFF后重新置0.

*4、校验码
	校验码为CRC(16位)校验，其运算范围为除STX/ETX/VERIFY之外的所有数据区，运算结果为2字节的校验码，其中高位在前，低位在后。
	运算区域如图1-3所示，
	备注：校验数据均为未经过转译处理的原始数据。
	LEN	SN	TOPCMD	SUBCMD	DATA
*5、特殊字符转义处理
	数据帧起始标志为DF(H)，帧截至标志为FD(H)。其他字段不能出现DF(H),FD(H)，如果数据确实为FD(H),DF(H)，需对其进行转义处理。
	转译影响的数据范围如图1-4所示：
	LEN	SN	TOPCMD	SUBCMD	DATA	VERIFY

	5.1发送数据处理（先校验，再转义）
	如果在待发送字段中出现DF(H)字节时，将DF(H)分解为DE(H)和01(H)这两个字节来发送；如果在待发送字段出现DE(H)字节时，需将DE(H)分解为DE(H)和00(H)这两个字节来发送。
	如果在待发送字段中出现FD(H)字节时，将FD(H)分解为FC(H)和01(H)这两个字节来发送；如果在待发送字段出现FC(H)字节时，需将FC(H)分解为FC(H)和00(H)这两个字节来发送。
	5.2接收数据处理（先转义，再校验）
	如果出现“DE 01”这样连续两个字节时将之合为一个字节DF(H)；如果出现“DE 00”这样连续两个字节时将之合为一个字节DE(H)。
	如果出现“FC 01”这样连续两个字节时将之合为一个字节FD(H)；如果出现“FC 00”这样连续两个字节时将之合为一个字节FC(H)。
	智能扫码器送上来的所有整型数据，未特定说明，其字节排序均为高位在前，低位在后。
*/

#pragma once 

#include "CommandDef.h"
#include <QByteArray>

class CommonPacketObj
{
public:
    CommonPacketObj();

public:
    //数据处理
    static bool CheckPackage(const std::vector<uint8>& vecData);     //CRC校验判断
    static vector<uint8> CRC_16(vector<uint8> &byte, uint32 len);     //CRC校验
    static vector<uint8> unEscapeComProtocol(vector<uint8> &lpData, uint32 len, uint32 bSendState);   //转译，bSendState：1发送，0接收
    //编码格式转换
    static vector<uint8> VectorUTF8ToGBK(char *msg, int len);
    static QByteArray BytesUTF8ToGBK(char *msg, int len);
    static vector<uint8> VectorGBKToUTF8(char *msg, int len);
    static QByteArray BytesGBKToUTF8(char *msg, int len);
    //packet打包
    static vector<uint8> PackBytes(DevFunctionType1 actionType, uint8 objectType, vector<uint8>& objectData);
    static vector<uint8> PackBytes(uint8 actionType, uint8 objectType, char *msg, int len);
    //unpacket解包
    static vector<vector<uint8>> UnPackBytes(vector<uint8>& data);
};
