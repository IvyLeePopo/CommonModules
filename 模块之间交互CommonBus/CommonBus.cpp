#include "CommonBus.h"
#include "CommonBusSource.h"
#include <stdio.h>
#include <QDebug>
#include <QJsonDocument>

CommonBus::CommonBus()
{

}

CommonBus::~CommonBus()
{

}

//注册key
void CommonBus::register2Bus(int key, QString clssName) //openMsgQueyeId
{
	CommonMap::iterator myMapIt = CommonBusSource::myMap.find(key);
	if (myMapIt != CommonBusSource::myMap.end())
	{
		qDebug() << __LINE__ << __FUNCTION__ << QString("This key%1 (%2) is exsited,can't add!").arg(key).arg(clssName);
		return;
	}
	else
	{
		qDebug() << __LINE__ << __FUNCTION__ << QString("This key=%1 (%2) register successful!").arg(key).arg(clssName);
		CommonBusSource::myMap.insert(PairCommonMap(key, this));
	}
}

//注销key
void CommonBus::unRegister2Bus(int key, QString clssName)//openMsgQueyeId
{
	CommonMap::iterator myMapIt = CommonBusSource::myMap.find(key);
	if (myMapIt != CommonBusSource::myMap.end())
	{
		CommonBusSource::myMap.erase(myMapIt);
		qDebug() << __LINE__ << __FUNCTION__ << QString("Have removed key %1 (%2)").arg(key).arg(clssName);
	}
}

//消息发送入口
int CommonBus::send2Bus(int destKey, char *msg, int len, int srcKey) //send
{
	CommonMap::iterator myMapIt = CommonBusSource::myMap.find(destKey);
	if (myMapIt != CommonBusSource::myMap.end())
	{
		CommonBus *baseServer = myMapIt->second;
		return baseServer->recvFromBus(destKey, msg, len, srcKey);
	}
	else
	{
		qDebug() << __LINE__ << __FUNCTION__ << QString("This key %1 had unload !").arg(destKey);
		return -1;
	}
	return len;
}

//消息发送出口
int CommonBus::recvFromBus(int destKey, char *msg, int len, int srcKey) //recv
{
	Q_UNUSED(destKey);
	Q_UNUSED(msg);
	Q_UNUSED(len);
	Q_UNUSED(srcKey);
	return 0;
}
