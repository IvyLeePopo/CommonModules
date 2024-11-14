#ifndef AMYTHREADBASE_H
#define AMYTHREADBASE_H
//------------------------------------------------------------------
#include <QObject>
#include <QThread>
//------------------------------------------------------------------
class ATriggerEvent
{
private:
	int m_iStatus;
public:
	ATriggerEvent();

	void waitFor(int iMSecond);

	void resetEvent();
	void setEvent();
};
//------------------------------------------------------------------
class AMyThreadBase : public QThread
{
public:
	AMyThreadBase(QObject * parent = 0);
	virtual ~AMyThreadBase();

	void stop();
	void triggerNow();
protected:
	bool m_bStop;
	virtual void run();

	ATriggerEvent *m_pEvent;
	void waitFor(int iMSecond);
};
//------------------------------------------------------------------
#endif // AMYTHREADBASE_H
