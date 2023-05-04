/*
 * @Description: 模块的key，等等一些宏定义
 * @Author: Ivy
 * @Date: 2022-05-04 09:34:43
 * @LastEditTime: 2022-05-04 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

enum TW_PROT_CONST
{
	TW_PROT_HEADER = 0xDF,	            //协议起始符
	TW_PROT_END = 0xFD,                 //协议结束符
	TW_PROT_CONVERT_CHAR = 0x00,        //协议转译公共字符
	TW_PROT_HEAND_CONVERT_CHAR1 = 0xDE,	//协议起始符转译字符1
	TW_PROT_HEAND_CONVERT_CHAR2 = 0x01,	//协议起始符转译字符2
	TW_PROT_END_CONVERT_CHAR1 = 0xFC,	//协议休止符转译字符1
	TW_PROT_END_CONVERT_CHAR2 = 0x01	//协议休止符转译字符2
};

 //注册类KEY
enum ClassMapKeyValue
{
	KEY_HENAN_WEB_SOCKET_SERVER = 404,      //中间件通信服务端
	KEY_HENAN_HARDWARE_CTRL = 405,          //机器人硬件控制类
	KEY_SELF_HELP_ENTRY_NODE_MANAGER = 406, //自助发卡类
	KEY_SELF_HELP_EXIT_NODE_MANAGER = 407,  //自助缴费类
	KEY_READ_CARD_NODE_MANAGER = 408,       //读卡器类
};