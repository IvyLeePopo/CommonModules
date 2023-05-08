#pragma once

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
	SUB_CMD_TYPE_OpenReader = 0x01,			//打开 IC 卡读写器
	SUB_CMD_TYPE_CloseReader = 0x02,		//关闭 IC 卡读写器
}eSubCmdType_t;