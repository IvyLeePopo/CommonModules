#pragma once

#define MSG_HEAD_LEN    4    //帧头的字节长度（02 03 04 05）
#define MSG_LENGTH_LEN  4    //表示帧长度的数值的字节长度（XX XX XX XX，unsigned int型）
#define MSG_TAIL_LEN    1    //帧尾的字节长度（03）
#define MSG_FRONT_LEN   MSG_HEAD_LEN + MSG_LENGTH_LEN    //单帧DATA域之前的字节长度
#define MSG_MIN_LEN     MSG_HEAD_LEN + MSG_LENGTH_LEN + MSG_TAIL_LEN    //单帧最小字节长度

#define TCP_HEART_BEAT_INTERVAL 10 * 1000  //心跳包发送间隔
#define TCP_RECONNECT_INTERVAL 10 * 1000  //发起重连间隔

typedef unsigned char           uint8;
typedef unsigned short          uint16;
typedef unsigned int            uint32;
typedef unsigned long long      uint64;

//数据帧定义解析
typedef enum ePROT_CONST
{
	PROT_HEADER = 0xDF,						//协议起始符
	PROT_END = 0xFD,						//协议结束符
	PROT_NUMBER = 0x00,						//协议帧编号
	PROT_CONVERT_CHAR = 0x00,				//协议转译公共字符
	PROT_HEAND_CONVERT_CHAR1 = 0xDE,		//协议起始符转译字符1
	PROT_HEAND_CONVERT_CHAR2 = 0x01,		//协议起始符转译字符2
	PROT_END_CONVERT_CHAR1 = 0xFC,			//协议休止符转译字符1
	PROT_END_CONVERT_CHAR2 = 0x01			//协议休止符转译字符2
}ePROT_CONST_t;

//主命令类型
typedef enum eTopCmdType
{
	TOP_CMD_TYPE = 0x58       //主命令类型，帧标记，其值:58(H)
}eTopCmdType_t;

//子命令类型
typedef enum eSubCmdType
{	
	//TOPCMD:58(H)
	SUB_CMD_TYPE_OpenReader = 0x01,			//打开
	SUB_CMD_TYPE_CloseReader = 0x02,		//关闭
}eSubCmdType_t;