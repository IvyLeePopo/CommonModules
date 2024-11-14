#include "aeasylog.h"

#include <QDir>
#include <QMutexLocker>
#include <QDateTime>
#include <sys/time.h>

//---------------------------------------------------------
QString AEasyLog::m_sPrefix = "";
QString AEasyLog::m_sLogPath = "./Log/";
int AEasyLog::m_iWriteLevel = 0;
int AEasyLog::m_iCode = 0; //编码格式 0：Utf-8 1:GBK
//QMutex AEasyLog::m_mutex(QMutex::Recursive);

AEasyLog * g_pLog = NULL;
//---------------------------------------------------------
AEasyLog::AEasyLog()
	:QObject(NULL),m_mutex(QMutex::Recursive)
{
	connect(this,SIGNAL(sigToWriteLog()),this,SLOT(sltOnWriteLog()), Qt::QueuedConnection);

	m_thread.start();
	this->moveToThread(&m_thread);
}

AEasyLog::~AEasyLog()
{
	m_thread.quit();
	m_thread.wait();

	toWriteLog(LL_DEBUG,"AEasyLog::~AEasyLog()");
	sltOnWriteLog();
}
//---------------------------------------------------------
void AEasyLog::setPrefix(QString sPrefix)
{
	AEasyLog::m_sPrefix = sPrefix;
}
//---------------------------------------------------------
void AEasyLog::setPath(QString sPath)
{
	AEasyLog::m_sLogPath = sPath;
	if(m_sLogPath.endsWith("/")==false || m_sLogPath.endsWith("\\"))
		m_sLogPath += "/";

	QDir dir(m_sLogPath);
	if(dir.exists()==false)
	{
		QDir dircur;
		dircur.mkpath(m_sLogPath);
	}
}
//---------------------------------------------------------
void AEasyLog::setManageLogInfo(QString sLogPath, int iStayDay, bool bNeedZip, QString Filters)
{
    if(sLogPath.endsWith("/")==false || sLogPath.endsWith("\\"))
        sLogPath += "/";

    AManageLogThread::getInstance().setManageLogInfo(sLogPath,iStayDay,bNeedZip,Filters);
}
//---------------------------------------------------------
void AEasyLog::setManageLogMode(int nMode, QString sDelJudgeHour)
{
    AManageLogThread::getInstance().setManageLogMode(nMode,sDelJudgeHour);
}

//---------------------------------------------------------
void AEasyLog::setWriteLevel(int iWriteLevel)
{
    AEasyLog::m_iWriteLevel = iWriteLevel;
}

void AEasyLog::setCode(int iCode)
{
    AEasyLog::m_iCode = iCode;
}
//---------------------------------------------------------
void AEasyLog::writeLog(int iLevel, const char *msg)
{
    if(iLevel<AEasyLog::m_iWriteLevel)
        return;

	AEasyLog::writeLog(iLevel,QString::fromLocal8Bit(msg));
}
//---------------------------------------------------------
void AEasyLog::writeLog(int iLevel, const QString &sMsg)
{
	if(NULL != g_pLog)
		g_pLog->toWriteLog(iLevel,sMsg);
}
//---------------------------------------------------------
void AEasyLog::init()
{
	if(NULL == g_pLog)
		g_pLog = new AEasyLog();
}
//---------------------------------------------------------
void AEasyLog::release()
{
	if(NULL != g_pLog) delete g_pLog; g_pLog = NULL;
}
//---------------------------------------------------------
void AEasyLog::toWriteLog(int iLevel, const QString &sMsg)
{
	if(iLevel<AEasyLog::m_iWriteLevel)
		return;

    //若日志目录不存在则去创建
    QDir dir(m_sLogPath);
    if(dir.exists()==false)
    {
        QDir dircur;
        dircur.mkpath(m_sLogPath);
    }

	struct timeval stCur;
	gettimeofday(&stCur, NULL);
	QDateTime dtCur = QDateTime::fromTime_t(stCur.tv_sec);
    QString sFileName = m_sLogPath + m_sPrefix + dtCur.toString("yyyyMMdd.log");
	QString sLogPrefix = dtCur.toString("hh:mm:ss")
					   + QString::asprintf(".%03d",(int)(stCur.tv_usec/1000))
					   + " " + getLogLevelStr(iLevel) + " ";

	QString sThreadName = QThread::currentThread()->objectName();
	if(sThreadName.isEmpty())
		sThreadName = QString::asprintf("0x%08X", (quintptr)QThread::currentThreadId());

	QMutexLocker locker(&m_mutex);
	m_qmLogMsg[sFileName].push_back(sLogPrefix + "[" + sThreadName + "]" + sMsg + "\r\n");

	emit sigToWriteLog();
}
//---------------------------------------------------------
QString AEasyLog::getLogLevelStr(int iLevel)
{
	QString sLogLevel = "[U]";
	switch (iLevel)
	{
	case LL_TRACE:
		sLogLevel = "[T]";
		break;
	case LL_DEBUG:
		sLogLevel = "[D]";
		break;
	case LL_INFO:
		sLogLevel = "[I]";
		break;
	case LL_WARN:
		sLogLevel = "[W]";
		break;
	case LL_ERROR:
		sLogLevel = "[E]";
		break;
	case LL_FATAL:
		sLogLevel = "[F]";
		break;
	case LL_NUL:
		sLogLevel = "[ ]";
	}

	return sLogLevel;
}
//---------------------------------------------------------
void AEasyLog::writeLogFile(const QString &sFileName, QList<QString> &qlLogMsg)
{
	QFile qfLog(sFileName);
	if(qfLog.open(QIODevice::Append)==false)
		return;

	while(!qlLogMsg.empty())
	{
		qfLog.write(qlLogMsg.front().toLocal8Bit());
		qlLogMsg.pop_front();
	}
	qfLog.close();
}
//---------------------------------------------------------
void AEasyLog::sltOnWriteLog()
{
	QMap<QString,QList<QString> > qmLogMsg;
	{
		QMutexLocker locker(&m_mutex);
		qmLogMsg.swap(m_qmLogMsg);
	}

	QMap<QString,QList<QString> >::iterator it;
	for(it=qmLogMsg.begin();it!=qmLogMsg.end();++it)
	{
		writeLogFile(it.key(),it.value());
	}
}
//---------------------------------------------------------
void logTrace(const QString &msg)
{
	AEasyLog::writeLog(AEasyLog::LL_TRACE,msg);
}

void logTrace(const char *msg,...)
{
	va_list ap;
	va_start(ap,msg);
    QString sMsgTmp;
    if(AEasyLog::LOGCODE::LC_UTF8 == AEasyLog::m_iCode)
        sMsgTmp = TOUTF8(msg);
    else
        sMsgTmp = TOGBK(msg);
	QString sLogMsg = QString::vasprintf(sMsgTmp.toUtf8().data(),ap);
	va_end(ap);

	logTrace(sLogMsg);
}
//---------------------------------------------------------
void logDebug(const QString &msg)
{
	AEasyLog::writeLog(AEasyLog::LL_DEBUG,msg);
}

void logDebug(const char *msg,...)
{
	va_list ap;
	va_start(ap,msg);
    QString sMsgTmp;
    if(AEasyLog::LOGCODE::LC_UTF8 == AEasyLog::m_iCode)
        sMsgTmp = TOUTF8(msg);
    else
        sMsgTmp = TOGBK(msg);
	QString sLogMsg = QString::vasprintf(sMsgTmp.toUtf8().data(),ap);
	va_end(ap);

	logDebug(sLogMsg);
}
//---------------------------------------------------------
void logInfo(const QString &msg)
{
	AEasyLog::writeLog(AEasyLog::LL_INFO,msg);
}

void logInfo(const char *msg,...)
{
	va_list ap;
	va_start(ap,msg);
    QString sMsgTmp;
    if(AEasyLog::LOGCODE::LC_UTF8 == AEasyLog::m_iCode)
        sMsgTmp = TOUTF8(msg);
    else
        sMsgTmp = TOGBK(msg);
	QString sLogMsg = QString::vasprintf(sMsgTmp.toUtf8().data(),ap);
	va_end(ap);

	logInfo(sLogMsg);
}
//---------------------------------------------------------
void logWarn(const QString &msg)
{
	AEasyLog::writeLog(AEasyLog::LL_WARN,msg);
}

void logWarn(const char *msg,...)
{
	va_list ap;
	va_start(ap,msg);
    QString sMsgTmp;
    if(AEasyLog::LOGCODE::LC_UTF8 == AEasyLog::m_iCode)
        sMsgTmp = TOUTF8(msg);
    else
        sMsgTmp = TOGBK(msg);
	QString sLogMsg = QString::vasprintf(sMsgTmp.toUtf8().data(),ap);
	va_end(ap);

	logWarn(sLogMsg);
}
//---------------------------------------------------------
void logError(const QString &msg)
{
	AEasyLog::writeLog(AEasyLog::LL_ERROR,msg);
}

void logError(const char *msg,...)
{
	va_list ap;
	va_start(ap,msg);
    QString sMsgTmp;
    if(AEasyLog::LOGCODE::LC_UTF8 == AEasyLog::m_iCode)
        sMsgTmp = TOUTF8(msg);
    else
        sMsgTmp = TOGBK(msg);
	QString sLogMsg = QString::vasprintf(sMsgTmp.toUtf8().data(),ap);
	va_end(ap);

	logError(sLogMsg);
}
//---------------------------------------------------------
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	Q_UNUSED(context)

	switch(type)
	{
	case QtDebugMsg:
		logDebug(msg);
		break;
	case QtInfoMsg:
		logInfo(msg);
		break;
	case QtWarningMsg:
		logWarn(msg);
		break;
	case QtCriticalMsg:
	case QtFatalMsg:
		logError(msg);
		break;
	default:
		logTrace(msg);
		break;
	}
}

void myConsoleOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	Q_UNUSED(type)
	Q_UNUSED(context)

	static QMutex mutex;
	QMutexLocker locker(&mutex);

	fprintf(stderr,"%s\r\n", msg.toLocal8Bit().data());

	logDebug(msg.toLocal8Bit().data());
}
//---------------------------------------------------------

void logRemote(const QString &msg)
{
    Q_UNUSED(msg)
}

void logRemote(const char *msg,...)
{
	va_list ap;
	va_start(ap,msg);
    QString sMsgTmp;
    if(AEasyLog::LOGCODE::LC_UTF8 == AEasyLog::m_iCode)
        sMsgTmp = TOUTF8(msg);
    else
        sMsgTmp = TOGBK(msg);
	QString sLogMsg = QString::vasprintf(sMsgTmp.toUtf8().data(),ap);
	va_end(ap);

	logError(sLogMsg);
}
//------------------------------------------------------------------
//ALoger
//------------------------------------------------------------------
 ALoger::ALoger(int loglevel)
 {
     m_LogLevel=loglevel;
 }
 ALoger::~ALoger()
 {
     AEasyLog::writeLog(m_LogLevel,m_strMsg);
 }
 void ALoger::pushLog(const QString& str)
 {
     if(m_strMsg.isEmpty())
        m_strMsg += str;
     else
        m_strMsg +=(" "+str);
 }

 QSharedPointer<ALoger> operator <<(QSharedPointer<ALoger> log,const QString& str)
 {
     log->pushLog(str);
     return log;
 }

 QSharedPointer<ALoger> operator <<(QSharedPointer<ALoger> log,const QByteArray& str)
 {
     log->pushLog(str);
     return log;
 }

 QSharedPointer<ALoger> operator <<(QSharedPointer<ALoger> log,const char* str)
 {
     log->pushLog(QString::fromLocal8Bit(str));
     return log;
 }
 QSharedPointer<ALoger> operator <<(QSharedPointer<ALoger> log,int value)
 {
     log->pushLog(QString::number(value));
     return log;
 }

 QSharedPointer<ALoger> operator <<(QSharedPointer<ALoger> log, const QVariantMap &mapData)
 {
     QString sMsg;
     for(auto it=mapData.cbegin();it!=mapData.cend();++it)
     {
         if(sMsg.length()>0) sMsg += ",";
         sMsg += it.key() + "=" + it.value().toString();
     }
     log->pushLog(sMsg);
     return log;
 }

 QSharedPointer<ALoger> operator <<(QSharedPointer<ALoger> log, const QMap<QString, QString>&mapData)
 {
     QString sMsg;
     for(auto it=mapData.cbegin();it!=mapData.cend();++it)
     {
         if(sMsg.length()>0) sMsg += ",";
         sMsg += it.key() + "=" + it.value();
     }
     log->pushLog(sMsg);
     return log;
 }

 QSharedPointer<ALoger> logTrace()
 {
     return QSharedPointer<ALoger>(new ALoger(AEasyLog::LL_TRACE));
 }

 QSharedPointer<ALoger> logDebug()
 {
     return QSharedPointer<ALoger>(new ALoger(AEasyLog::LL_DEBUG));
 }

 QSharedPointer<ALoger> logInfo()
 {
     return QSharedPointer<ALoger>(new ALoger(AEasyLog::LL_INFO));
 }

 QSharedPointer<ALoger> logWarn()
 {
     return QSharedPointer<ALoger>(new ALoger(AEasyLog::LL_INFO));
 }

 QSharedPointer<ALoger> logError()
 {
     return QSharedPointer<ALoger>(new ALoger(AEasyLog::LL_ERROR));
 }
//------------------------------------------------------------------
