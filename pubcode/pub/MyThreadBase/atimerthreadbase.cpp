#include "../pubCode/pub/MyThreadBase/atimerthreadbase.h"

#include <QEventLoop>
#include <QDebug>
//------------------------------------------------------------------
ATimerThreadBase::ATimerThreadBase(QObject *parent) : QObject(parent)
{
	m_pTimer = nullptr;
	m_bInitFin = false;
	m_thread.start();
	this->moveToThread(&m_thread);
    connect(this,SIGNAL(sigToInit()),this,SLOT(sltInit()),Qt::QueuedConnection);

	//保证线程已经起来了
	while (m_thread.isRunning()==false)
	{
		QThread::msleep(1);
	}

    emit sigToInit();
}
//------------------------------------------------------------------
ATimerThreadBase::~ATimerThreadBase()
{
	callDeinitInThread();

	m_thread.quit();
	m_thread.wait();
}
//------------------------------------------------------------------
void ATimerThreadBase::setThreadName(QString sThreadName)
{
	m_thread.setObjectName(sThreadName);
}

void ATimerThreadBase::start(int iMSec)
{
	m_iInterval = iMSec;
	emit sigStart();
}
//------------------------------------------------------------------
void ATimerThreadBase::stop()
{
	emit sigStop();
}
//------------------------------------------------------------------
void ATimerThreadBase::triggerNow()
{
	emit sigTriggerNow();
}
//------------------------------------------------------------------
void ATimerThreadBase::waitInitFin()
{
	while(false == m_bInitFin)
		QThread::msleep(10);
}
//------------------------------------------------------------------
void ATimerThreadBase::callDeinitInThread()
{
    QEventLoop loop;
    connect(this, SIGNAL(sigOndeinit()), &loop, SLOT(quit()));
	QTimer::singleShot(0, this, SLOT(sltDeinit()));
	loop.exec();
}
//------------------------------------------------------------------
void ATimerThreadBase::initInThread()
{
	//qDebug()<<"ATimerThreadBase::initInThread()";
}
//------------------------------------------------------------------
void ATimerThreadBase::deinitInThread()
{
	//qDebug()<<"ATimerThreadBase::deinitInThread()";
}
//------------------------------------------------------------------
void ATimerThreadBase::sltInit()
{
	connect(this,SIGNAL(sigTriggerNow()),this,SLOT(sltOnTimerWork()),Qt::QueuedConnection);
	connect(this,SIGNAL(sigStart()),this,SLOT(sltStart()),Qt::QueuedConnection);
	connect(this,SIGNAL(sigStop()),this,SLOT(sltStop()),Qt::QueuedConnection);

	//预留点时间给子类构造函数处理完，再进行初始化，防止使用错了的虚函数
    QThread::msleep(50);
	initInThread();
	m_bInitFin = true;
}
//------------------------------------------------------------------
void ATimerThreadBase::sltDeinit()
{
	if(nullptr != m_pTimer)
	{
		m_pTimer->stop();
		delete m_pTimer;
		m_pTimer = nullptr;
	}

	deinitInThread();
	emit sigOndeinit();
}
//------------------------------------------------------------------
void ATimerThreadBase::sltStart()
{
	if(nullptr == m_pTimer)
	{
		m_pTimer = new QTimer(this);
		connect(m_pTimer, SIGNAL(timeout()), this, SLOT(sltOnTimerWork()));
	}

	//开启时钟，定时执行
	m_pTimer->start(m_iInterval);
}
//------------------------------------------------------------------
void ATimerThreadBase::sltStop()
{
	if(nullptr == m_pTimer)
		return;

	//停止时钟
	m_pTimer->stop();
}
//------------------------------------------------------------------
void ATimerThreadBase::sltOnTimerWork()
{
	doThreadTimer();
}
//------------------------------------------------------------------
