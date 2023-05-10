
#include "pch.h"
#include "httpfunc.h"
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "application/applicationbase.h"
#include "application/pubglobalinstances.h"
#include "application/sdloginmanager.h"
#include "application/settingsmgr.h"
#include "common/global.h"
#include "datatransfermgr/serverpipesdk/resource-sdk.h"
#include "general/genericdef.h"
#include "general/genericfunc.h"
#include "general/jsonutils.h"
#include "replytimeout.h"
#include "report/reportinfomanager.h"
#include "urlmanager/httpurldef.h"

#define ELAPSE_TIMER            10000
#define PROP_CALLID             "callId"
#define AddCallbackObject() {QMutexLocker lock(&m_dataLock);\
                                CallbackInfo *callbackInfo = new CallbackInfo(object, reply, callback);\
                                callbackInfo->m_timeoutTimerId = this->startTimer(ELAPSE_TIMER);\
                                m_mapCallback[reply] = callbackInfo;}
namespace
{
	QString calKeySha1(QByteArray key, const QByteArray& baseString)
	{
		int blockSize = 32;
		if (key.length() > blockSize)
		{
			key = QCryptographicHash::hash(key, QCryptographicHash::Sha1);
		}
		QByteArray prePadding(blockSize, char(0x53)); //S
		QByteArray endPadding(blockSize, char(0x64)); //d
		for (int i = 0; i < key.length(); i++)
		{
			prePadding[i] = prePadding[i] ^ key.at(i);
			endPadding[i] = endPadding[i] | key.at(i);
		}
		QByteArray total = prePadding + baseString + endPadding;
		QByteArray hashed = QCryptographicHash::hash(total, QCryptographicHash::Sha1);
		return hashed.toHex().toBase64();
	}
}

HttpFunc::HttpFunc(QObject *parent)
	: QObject(parent)
	, m_callId(1)
{
	m_netWorkManager = new QNetworkAccessManager(this);
	//修复在部分电脑上不能使用http请求失败的问题“QIODevice::read (QDisabledNetworkReply): device not open”，“Error String : Network access is disabled”
	m_netWorkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);

	connect(m_netWorkManager, &QNetworkAccessManager::finished, this, &HttpFunc::onFinished, Qt::QueuedConnection);
	//QNetworkAccessManager::finished 该信号在等待的网络应答完成时发出。
	//reply参数将包含一个指针，指向刚刚完成的回复。该信号与QNetworkReply::finished()信号一起发出。
}

HttpFunc::~HttpFunc()
{
	if (m_netWorkManager)
	{
		// 增加这句，防止进程退出
		// QWaitCondition: Destroyed while threads are still waiting
		// 不用时要调用disconnect方法
		disconnect(m_netWorkManager, &QNetworkAccessManager::finished, this, &HttpFunc::onFinished);
		delete m_netWorkManager;
	}
}

void HttpFunc::clearAllTask()
{
	QMutexLocker lock(&m_dataLock);
	for (auto iter = m_mapCallback.begin(); iter != m_mapCallback.end(); )
	{
		// 当iter.value().m_callback为空时暂时认为是注销事件，不作处理
		if (iter.value()->m_networkReply && iter.value()->m_callback)
		{
			CallbackInfo  *callBackInfo = iter.value();
			QPointer<QNetworkReply> networkReply = callBackInfo->m_networkReply;
			killTimer(callBackInfo->m_timeoutTimerId);
			iter = m_mapCallback.erase(iter);
			// 调用abort的时候会直接触发onFinished信号，从而导致m_mapCallback被清
			if (networkReply)
			{
				networkReply->abort();
				networkReply->deleteLater();
			}
		}
		else
		{
			++iter;
		}
	}
}

quint64 HttpFunc::login(const QString &userName, const QString &password, QObject *object, HttpCallBackFunc callback, bool block)
{
	if (!m_netWorkManager)
		return 0;

	QString url = LOGIN_URL;
	QJsonObject jsonObject;
	jsonObject.insert("account", userName);
	jsonObject.insert("platform", "1");
	jsonObject.insert("terminal", "CLIENT");
	jsonObject.insert("password", password);
	jsonObject.insert("passwordEncrypted", true);
	jsonObject.insert("version", global::getVersion());
	jsonObject.insert("channelid", global::getChannelID());
	QByteArray dataArray = QJsonDocument(jsonObject).toJson();
	//设置消息头
	QNetworkRequest request = createNetworkRequest(url, HM_Post);
	request.setHeader(QNetworkRequest::ContentLengthHeader, dataArray.length());
	// 为统计各终端的新增注册用户数与每日登录用户数，ideation pc端添加"Sdtc-Terminal-Id"字段，
	// windows 值为201, macOS值为202

#ifdef Q_OS_WIN
	request.setRawHeader("Sdtc-Terminal-Id", QString("201").toUtf8().data());
#elif defined(Q_OS_MAC)
	request.setRawHeader("Sdtc-Terminal-Id", QString("202").toUtf8().data());
#endif

	//开始请求
	QNetworkReply *reply = m_netWorkManager->post(request, dataArray);
	if (block)
	{
		waitForHttpReturn(reply, callback, 5000);
	}
	else
	{
		AddCallbackObject();
	}
	return getNewCallId(reply);
}

quint64 HttpFunc::logout(const QString &token, QObject *object, HttpCallBackFunc callback, bool block)
{
	if (!m_netWorkManager)
		return 0;
	QString url = LOGOUT_URL;
	//设置消息头
	QNetworkRequest request = createNetworkRequest(token, url, HM_Post);
	request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
	//开始请求
	QNetworkReply *reply = m_netWorkManager->post(request, QByteArray());
	if (block)
	{
		waitForHttpReturn(reply, callback, 5000);
	}
	else
	{
		AddCallbackObject();
	}
	return getNewCallId(reply);
}

void HttpFunc::onFinished(QNetworkReply *networkReply)
{
	std::shared_ptr<HttpFunc::CallbackInfo> callbackInfo;
	{
		QMutexLocker lock(&m_dataLock);
		QMap<QNetworkReply *, CallbackInfo *>::iterator iter = m_mapCallback.find(networkReply);
		if (iter != m_mapCallback.end())
		{
			killTimer(iter.value()->m_timeoutTimerId);
			iter.value()->m_timeoutTimerId = -1;
			callbackInfo = std::shared_ptr<HttpFunc::CallbackInfo>(iter.value());
			m_mapCallback.erase(iter);
		}
		else
		{
			return;
		}
	}
	CheckResult(callbackInfo);
	quint32 httpCode = 0;
	QByteArray responseData;
	quint64 callId = 0;
	QPointer<QNetworkReply> reply = callbackInfo->m_networkReply;
	if (reply)
	{
		QVariant var = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
		httpCode = var.toInt();
		responseData = reply->readAll();
	}

	// iter.value()为空时暂时是认定为注销事件
	if (httpCode == 401 && callbackInfo->m_object)
	{
		// 这里需要先调用一下结果回调函数，否则可能会导致外面一直等待http请求返回，导致一些操作一直卡住没法继续进行
		// 如，有一个http请求，界面上在显示进度条等待结果返回时，就会导致界面一直等待，永远不会返回
		if (reply)
			callbackInfo->m_callback(reply->error(), httpCode, callId, responseData);

		SdApplicationInstance->userGrab();
		// 这个deleteLater操作不能放在这个大括号外面，因为这个操作如果是阻塞动作的话会自己调用deleteLater
		// 具体参考getOSSAuth方法
		if (reply)
		{
			reply->abort();
			reply->deleteLater();
		}
		return;
	}
	if (networkReply)
		callId = networkReply->property(PROP_CALLID).toLongLong();

	int errorCode = QNetworkReply::NoError;
	if (reply)
		errorCode = reply->error();

	if (errorCode != QNetworkReply::NoError || httpCode != 200)
	{
		LOG(ERROR);
	}

	if (callbackInfo->m_object)
		callbackInfo->m_callback(errorCode, httpCode, callId, responseData);
	// 这个deleteLater操作不能放在这个大括号外面，因为这个操作如果是阻塞动作的话会自己调用deleteLater
	// 具体参考getOSSAuth方法
	// timerEvent里也有加锁m_dataLock去掉m_mapCallback里的数据，所以这可能会导致reply已经delete了
	if (reply)
	{
		reply->abort();
		reply->deleteLater();
	}
}
void HttpFunc::timerEvent(QTimerEvent *event)
{
	int id = event->timerId();
	killTimer(id);
	QPointer<QObject> object;
	quint64 callId = 0;
	HttpCallBackFunc callback = nullptr;
	QPointer<QNetworkReply> networkReply;
	// 防止callback调用网络请求导致死锁
	{
		QMutexLocker lock(&m_dataLock);
		for (auto iter = m_mapCallback.begin(); iter != m_mapCallback.end(); ++iter)
		{
			if (iter.value()->m_timeoutTimerId == id)
			{
				LOG(ERROR) << "http fail:error=" << 408 << ",httpcode=" << 408 << ",errorStr=timeout" << iter.value()->m_networkReply->url().toString().toUtf8().data();
				object = iter.value()->m_object;
				callback = iter.value()->m_callback;
				networkReply = iter.value()->m_networkReply;
				delete iter.value();
				m_mapCallback.erase(iter);
				break;
			}
		}
	}
	if (networkReply)
	{
		callId = networkReply->property(PROP_CALLID).toLongLong();
		LOG(INFO) << "network delete: " << networkReply.data();
		networkReply->abort();
		networkReply->deleteLater();
	}
	if (object)
	{
		callback(408, 408, callId, "timeout");
	}
}

QNetworkRequest HttpFunc::createNetworkRequest(const QString &token, const QString &url, Http_Method method)
{
	Q_UNUSED(method);
	QNetworkRequest request;
	request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
	request.setRawHeader("Accept", "application/json,text/plain,*/*");
	if (!token.isEmpty())
		request.setRawHeader("token", token.toLatin1());
	// 多系统账户数据互通需要，加上Sdtc-Tenant-Id和Sdtc-App-Id字段
	if (SdLoginManagerInstance)
		request.setRawHeader("Sdtc-Tenant-Id", SdLoginManagerInstance->getCompanyId().toUtf8());

	request.setRawHeader("Sdtc-App-Id", QString("100").toUtf8());
	// 全埋点id，上传给后端，由后端关联静态属性信息
	if (ReportInfoManager::getInstance().isHadUniqueSessionId())
	{
		request.setRawHeader("unique_session_id", ReportInfoManager::getInstance().getUniqueSessionId().toUtf8());
		ReportInfoManager::getInstance().insertDynamicProperty(request);
	}
	request.setUrl(QUrl(url));
	return request;
}

void HttpFunc::finishCallback(QNetworkReply *reply, HttpCallBackFunc callback)
{
	QString value;
	quint32 httpCode = 0;
	QByteArray responseData;
	QVariant var = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	httpCode = var.toInt();
	responseData = reply->readAll();
	int errorCode = reply->error();
	quint64 callId = reply->property(PROP_CALLID).toLongLong();
	if (errorCode != QNetworkReply::NoError || httpCode != 200)
		return;

	if (callback)
		callback(errorCode, httpCode, callId, responseData);

	if (SdApplicationInstance && httpCode == 401)
		SdApplicationInstance->userGrab();
}

void HttpFunc::waitForHttpReturn(QNetworkReply *reply, HttpCallBackFunc callback, quint32 microsecond)
{
	ReplyTimeout *replyTimeout = new ReplyTimeout(reply, nullptr, HTP_Finish | HTP_Error, microsecond);

	if (replyTimeout->isTimeOut())
	{
		LOG(INFO) << "time out";
	}
	else
	{
		if (reply->error() != QNetworkReply::NoError)
		{
			QString strError = reply->errorString();
			LOG(INFO) << "http fail: " << strError.toUtf8().data();
		}
	}

	finishCallback(reply, callback);
	replyTimeout->deleteLater();
	disconnect(reply);
	reply->abort();
	reply->deleteLater();
}

quint64 HttpFunc::getNewCallId(QNetworkReply *reply)
{
	quint64 curCallId = m_callId++;
	if (reply)
	{
		reply->setProperty(PROP_CALLID, curCallId);
	}
	return curCallId;
}