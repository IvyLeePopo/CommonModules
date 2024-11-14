#include "amythreadbase.h"

#include <QDateTime>
#include <QElapsedTimer>
//------------------------------------------------------------------
ATriggerEvent::ATriggerEvent()
{
	m_iStatus = 0;
}
//------------------------------------------------------------------
void ATriggerEvent::waitFor(int iMSecond)
{
	m_iStatus = 1;
	QElapsedTimer timer;
	timer.start();

	qint64 iNow = timer.elapsed();
	while((iNow+iMSecond) >= timer.elapsed() && m_iStatus==1)
	{
		QThread::usleep(50);
	}
}
//------------------------------------------------------------------
void ATriggerEvent::resetEvent()
{
	m_iStatus = 0;
}
//------------------------------------------------------------------
void ATriggerEvent::setEvent()
{
	resetEvent();
}
//------------------------------------------------------------------
AMyThreadBase::AMyThreadBase(QObject * parent):QThread(parent)
{
	m_bStop = false;
	m_pEvent = new ATriggerEvent();
}
//------------------------------------------------------------------
AMyThreadBase::~AMyThreadBase()
{
	stop();
	if(m_pEvent!=NULL) delete m_pEvent; m_pEvent = NULL;
}
//------------------------------------------------------------------
void AMyThreadBase::stop()
{
	m_bStop = true;
	triggerNow();
	wait();
}
//------------------------------------------------------------------
void AMyThreadBase::triggerNow()
{
	m_pEvent->resetEvent();
}
//------------------------------------------------------------------
void AMyThreadBase::waitFor(int iMSecond)
{
	m_pEvent->waitFor(iMSecond);
}

//------------------------------------------------------------------
void AMyThreadBase::run()
{
	while(m_bStop==false)
	{
		//处理逻辑
		//...

		waitFor(1000);
	}
}

//------------------------------------------------------------------
