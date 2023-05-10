/*
 * @Description: 的用法
 * @Author: Ivy
 * @Date: 2022-05-10 09:34:43
 * @LastEditTime: 2022-05-10 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */


#pragma once
#include <QMap>
#include <QMutex>
#include <QNetworkReply>
#include <QObject>
#include <QPointer>
#include <functional>
#define bindHttpCallback(a) std::bind(&a, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
#define HttpCallBackFunc std::function<void(quint32, quint32, quint64, const QByteArray &)>
 // HttpFunc没有实现全局对象的原因是在部分模块中可能还没调用完HttpFunc就被销毁，
 // 从而HttpFunc的回调函数会调用失败，所以防止这种情况，只让这功能在哪个模块调用就在哪个模块声明

#define Q_OS_WIN 1

class HttpFunc : public QObject
{
	Q_OBJECT

public:
	// control对象用于在外面提供一个对象来控制超时控件的调用
	explicit HttpFunc(QObject *parent = nullptr);
	~HttpFunc();
	void clearAllTask();

public:
	enum Http_Method
	{
		HM_Get = 0,
		HM_Post,
		HM_Put,
	};

public:
	// 登录请求
	quint64 login(const QString &userName, const QString &password,	QObject *object, HttpCallBackFunc callback,	bool block = true);

	// 注销请求
	quint64 logout(const QString &token, QObject *object, HttpCallBackFunc callback, bool block = true);

protected slots:
	void onFinished(QNetworkReply *reply);

private:
	virtual void timerEvent(QTimerEvent *event)override;

private:
	QNetworkRequest createNetworkRequest(const QString &token, const QString &url, Http_Method method = HM_Get);

	// 完成事件回调，这个只在用户如果使用阻塞模式调用函数时才使用
	void finishCallback(QNetworkReply *reply, HttpCallBackFunc callback);
	void waitForHttpReturn(QNetworkReply *reply, HttpCallBackFunc callback, quint32 microsecond);
	quint64 getNewCallId(QNetworkReply *reply);

	class CallbackInfo
	{
	public:
		CallbackInfo()
			: m_callback(nullptr)
			, m_timeoutTimerId(-1){	}

		CallbackInfo(const CallbackInfo &info)
		{
			m_object = info.m_object;
			m_networkReply = info.m_networkReply;
			m_callback = info.m_callback;
		}

		CallbackInfo(QObject *object, QNetworkReply *networkReply, HttpCallBackFunc callback)
		{
			m_object = object;
			m_networkReply = networkReply;
			m_callback = callback;
		}

		CallbackInfo &operator =(const CallbackInfo &info)
		{
			m_object = info.m_object;
			m_networkReply = info.m_networkReply;
			m_callback = info.m_callback;
			return *this;
		}

		QPointer<QObject> m_object;
		QPointer<QNetworkReply> m_networkReply;
		HttpCallBackFunc m_callback;
		int     m_timeoutTimerId; // 请求超时的id
	};
private:
	QNetworkAccessManager *m_netWorkManager;
	QMap<QNetworkReply *, CallbackInfo *> m_mapCallback;
	QMutex              m_dataLock;
	// 请求用的id
	QAtomicInteger<quint64> m_callId;
};
