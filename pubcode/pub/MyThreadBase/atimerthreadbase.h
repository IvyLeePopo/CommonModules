//------------------------------------------------------------------
#ifndef ATIMERTHREADBASE_H
#define ATIMERTHREADBASE_H
//------------------------------------------------------------------
#include <QObject>
#include <QThread>
#include <QTimer>
//------------------------------------------------------------------
#ifndef TOGBK

#ifdef __GBKUSEUTF8
#define TOGBK(x) QString::fromUtf8(x)
#else
#define TOGBK(x) QString::fromLocal8Bit(x)
#endif

#endif

#ifndef GBK_LOG

#ifdef __GBKUSEUTF8
#define GBK_LOG(x) QString::fromUtf8(x).toUtf8().data()
#else
#define GBK_LOG(x) QString::fromLocal8Bit(x).toUtf8().data()
#endif

#endif

#ifndef GBK

#ifdef __GBKUSEUTF8
#define GBK(x) QString::fromUtf8(x)
#else
#define GBK(x) QString::fromLocal8Bit(x)
#endif

#endif
//------------------------------------------------------------------
class ATimerThreadBase : public QObject
{
	Q_OBJECT
public:
	explicit ATimerThreadBase(QObject *parent = 0);
	virtual ~ATimerThreadBase();

	void setThreadName(QString sThreadName);
	void start(int iMSec=50);
	void stop();
	void triggerNow();
	void waitInitFin();
	void callDeinitInThread();
protected:
	virtual void initInThread();
    virtual void doThreadTimer(){}
	virtual void deinitInThread();
protected:
	QThread m_thread;
	QTimer *m_pTimer;
	int m_iInterval;
	bool m_bInitFin;
signals:
    void sigToInit();
	void sigOndeinit();
	void sigStart();
	void sigStop();
	void sigTriggerNow();
protected slots:
	void sltInit();
	void sltDeinit();

	void sltStart();
	void sltStop();
	void sltOnTimerWork();
};
//------------------------------------------------------------------
#endif // ATIMERTHREADBASE_H
