#include "pch.h"
#include "replytimeout.h"
#include <QApplication>
#include "thread/taskbase.h"

ReplyTimeout::ReplyTimeout(QNetworkReply *reply, TaskBase *task, quint32 timeoutParam, const int timeout, QObject *parent)
	: QObject(parent)
	, m_bTimeOut(false)
{
	QTimer timer;
	timer.setInterval(timeout);
	timer.setSingleShot(true);
	QEventLoop loop;
	connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
	connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, &loop, &QEventLoop::quit);
	if (reply)
	{
		if (timeoutParam & HTP_Finish)
			connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
		if (timeoutParam & HTP_Read)
			connect(reply, &QNetworkReply::readyRead, &loop, &QEventLoop::quit);
		if (timeoutParam & HTP_Write)
			connect(reply, &QNetworkReply::bytesWritten, &loop, &QEventLoop::quit);
		if (timeoutParam & HTP_Error)
			connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), &loop, &QEventLoop::quit);
	}

	if (task)
	{
		connect(task, &TaskBase::_pause, &loop, &QEventLoop::quit);
		connect(task, &TaskBase::_exit, &loop, &QEventLoop::quit);
	}
	timer.start();
	loop.exec();
	if (timer.isActive())
	{
		m_bTimeOut = false;
		timer.stop();
	}
	else
	{
		m_bTimeOut = true;
	}
	if (reply)
	{
		if (timeoutParam & HTP_Finish)
			disconnect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
		else if (timeoutParam & HTP_Read)
			disconnect(reply, &QNetworkReply::readyRead, &loop, &QEventLoop::quit);
		else if (timeoutParam & HTP_Write)
			disconnect(reply, &QNetworkReply::bytesWritten, &loop, &QEventLoop::quit);
		else if (timeoutParam & HTP_Error)
			disconnect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), &loop, &QEventLoop::quit);
	}
	if (task)
	{
		disconnect(task, &TaskBase::_pause, &loop, &QEventLoop::quit);
		disconnect(task, &TaskBase::_exit, &loop, &QEventLoop::quit);
	}
}

bool ReplyTimeout::isTimeOut()
{
	return m_bTimeOut;
}
