/*
 * @Description: 控制超时控件
 * @Author: Ivy
 * @Date: 2022-05-10 09:34:43
 * @LastEditTime: 2022-05-10 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */


#pragma once
#include <QObject>
#include <QTimer>
#include "genericdef.h"
class TaskBase;
class ReplyTimeout : public QObject
{
	Q_OBJECT
public:
	// control对象用于在外面提供一个对象来控制超时控件的调用
	explicit ReplyTimeout(QNetworkReply *reply, TaskBase *task, quint32 timeoutParam = HTP_Finish, int timeout = HTTP_TIMEOUTDURATION, QObject *parent = nullptr);
	bool isTimeOut();
private:
	bool m_bTimeOut;
};
