#pragma once

//数据帧定义解析
typedef enum eProtocolConst
{
	PROTOCOL_HEADER = 0xDF,					//协议起始符
	PROTOCOL_END = 0xFD,					//协议休止符

	PROTOCOL_NUMBER = 0x00,			//协议帧编号

	PROTOCOL_CONVERT_CHAR = 0x00,			//协议转译公共字符

	PROTOCOL_HEAND_CONVERT_CHAR1 = 0xDE,	//协议起始符转译字符1
	PROTOCOL_HEAND_CONVERT_CHAR2 = 0x01,	//协议起始符转译字符2

	PROTOCOL_END_CONVERT_CHAR1 = 0xFC,		//协议休止符转译字符1
	PROTOCOL_END_CONVERT_CHAR2 = 0x01		//协议休止符转译字符2
}eProtocolType_t;

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
	SUB_CMD_TYPE_SetInitPPS = 0x03,			//设置 IC 卡读写器通讯速率（可选）
	SUB_CMD_TYPE_SetInitTimeOut = 0x04,     //设置 IC 卡读写器超时时间（可选）
	SUB_CMD_TYPE_LEDDisplay = 0x05,         //设置读写器发光二极管
	SUB_CMD_TYPE_AudioControl = 0x06,       //设置读写器蜂鸣器
	SUB_CMD_TYPE_GetStatus = 0x07,          //获取设备状态
	SUB_CMD_TYPE_ReaderVersion = 0x08,      //获取设备版本信息
	SUB_CMD_TYPE_GetVersion = 0x09,         //获取设备动态库版本
	SUB_CMD_TYPE_GetStatusMsg = 0x10,       //获取错误码详细描述
	SUB_CMD_TYPE_OpenCard = 0x11,           //打开卡片
	SUB_CMD_TYPE_CloseCard = 0x12,          //关闭卡片
	SUB_CMD_TYPE_CPUCommand = 0x13,         //CPU 卡通用指令函数
	SUB_CMD_TYPE_SamReset = 0x14,           //SAM 卡复位
	SUB_CMD_TYPE_SamCommand = 0x15,         //SAM 卡通用指令函数
	SUB_CMD_TYPE_LoadKey = 0x16,            //M1 卡载入密钥（可选）
	SUB_CMD_TYPE_ReadBlock = 0x17,          //M1 卡读块函数（可选）
	SUB_CMD_TYPE_WriteBlock = 0x18,         //M1 卡写块函数（可选）
}eSubCmdType_t;