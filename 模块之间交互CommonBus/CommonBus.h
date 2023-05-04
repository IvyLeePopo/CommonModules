/*
 * @Description: 主要是模块之间的交互，解耦
 * @Author: Ivy
 * @Date: 2022-05-04 09:34:43
 * @LastEditTime: 2022-05-04 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#ifndef COMMONBUS_H
#define COMMONBUS_H

#if defined(WIN32) || defined(WIN64) || defined(_WINDOWS)
#if defined(COMMONBUS_LIBRARY)
#define COMMONBUSSHARED_EXPORT __declspec(dllexport)
#else
#define COMMONBUSSHARED_EXPORT __declspec(dllimport)
#endif
#else
#define COMMONBUSSHARED_EXPORT
#endif

#include <QJsonObject>
#include "CommandDef.h"//是一些宏定义

class  COMMONBUSSHARED_EXPORT  CommonBus
{

public:
	CommonBus();
	virtual ~CommonBus();
	//注册  key:代表组件在全局定义的唯一键值
	virtual void register2Bus(int key, QString clssName = "");
	//注销  key:代表组件在全局定义的唯一键值
	virtual void unRegister2Bus(int key, QString clssName = "");

	/*
	发布
	destKey:代表目标组件在全局定义的唯一键值
	msg:需要发送的消息内容
	len:需要发送的消息内容的长度
	srcKey：本组件的全局定义的唯一键值。默认为0。
	*/
	virtual int send2Bus(int destKey, char *msg, int len, int srcKey = 0);

	/*
	订阅
	destKey:代表本组件的全局定义的唯一键值
	msg:需要发送的消息内容
	len:需要发送的消息内容的长度
	srcKey：代表源组件在全局定义的唯一键值
	*/
	virtual int recvFromBus(int destKey, char *msg, int len, int srcKey = 0);

public:
	int commpentsKey;
};


#endif // COMMONBUS_H
