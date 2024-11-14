#ifndef AEASYLOG_H
#define AEASYLOG_H
//---------------------------------------------------------
#include <QObject>
#include <QThread>
#include <QMutex>
#include <QMap>
#include <QList>
#include <QSharedPointer>
#include <QVariantMap>

#include "amanagelogthread.h"

#define TOUTF8(x) QString::fromUtf8(x)
#define TOGBK(x) QString::fromLocal8Bit(x)

#define GBK_LOG(x) QString::fromUtf8(x).toUtf8().data()
#define UTF8_LOG(x) QString::fromLocal8Bit(x).toUtf8().data()

#define UTF8(x) QString::fromUtf8(x)
#define GBK(x) QString::fromLocal8Bit(x)



//---------------------------------------------------------
class AEasyLog :public QObject
{
	Q_OBJECT
public:
	AEasyLog();
	~AEasyLog();

	static void setPrefix(QString sPrefix);
	static void setPath(QString sPath);
    static void setManageLogInfo(QString sLogPath,int iStayDay = 60,bool bNeedZip = false, QString Filters = "*.log|*.txt");
    static void setManageLogMode(int nMode,QString sDelJudgeHour = "02");
	static void setWriteLevel(int iWriteLevel);
    static void setCode(int iCode);

	static void writeLog(int iLevel,const char* msg);
	static void writeLog(int iLevel,const QString &sMsg);

	static void init();
	static void release();

	static QString getLogLevelStr(int iLevel);
public:
    enum LOGLEVEL
    {
        LL_TRACE=0, LL_DEBUG, LL_INFO, LL_WARN, LL_ERROR,LL_FATAL,LL_NUL
    };

    enum LOGCODE
    {
        LC_UTF8=0, LC_GBK
    };

    static int m_iCode;
	static int m_iWriteLevel;
	static QString m_sLogPath;
	static QString m_sPrefix;
private:
	QThread m_thread;
	QMutex m_mutex;
	QMap<QString,QList<QString> > m_qmLogMsg;	//[文件名] = 日志内容列表

	void toWriteLog(int iLevel,const QString &sMsg);
	void writeLogFile(const QString &sFileName, QList<QString> &qlLogMsg);
signals:
	void sigToWriteLog();
private slots:
	void sltOnWriteLog();
};
extern AEasyLog * g_pLog;
//---------------------------------------------------------
extern void logTrace(const QString &msg);
extern void logTrace(const char* msg,...);
extern void logDebug(const QString &msg);
extern void logDebug(const char* msg,...);
extern void logInfo(const QString &msg);
extern void logInfo(const char* msg,...);
extern void logWarn(const QString &msg);
extern void logWarn(const char* msg,...);
extern void logError(const QString &msg);
extern void logError(const char* msg,...);

extern void logRemote(const QString &msg);
extern void logRemote(const char* msg,...);

extern void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
extern void myConsoleOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
//---------------------------------------------------------
class ALoger
{
public:
    ALoger(int loglevel);
    ~ALoger();
    void pushLog(const QString& str);
private:
    int m_LogLevel;
    QString m_strMsg;
};
extern  QSharedPointer<ALoger> operator <<(QSharedPointer<ALoger> log,const QString& str);
extern  QSharedPointer<ALoger> operator <<(QSharedPointer<ALoger> log,const QByteArray& str);
extern  QSharedPointer<ALoger> operator <<(QSharedPointer<ALoger> log,const char* str);
extern  QSharedPointer<ALoger> operator <<(QSharedPointer<ALoger> log,int value);
extern  QSharedPointer<ALoger> operator <<(QSharedPointer<ALoger> log,const QVariantMap& mapData);
extern  QSharedPointer<ALoger> operator <<(QSharedPointer<ALoger> log,const QMap<QString, QString>&mapData);
extern  QSharedPointer<ALoger> logTrace();
extern  QSharedPointer<ALoger> logDebug();
extern  QSharedPointer<ALoger> logInfo();
extern  QSharedPointer<ALoger> logWarn();
extern  QSharedPointer<ALoger> logError();
//---------------------------------------------------------
#endif // AEASYLOG_H
