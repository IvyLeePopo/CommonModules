#include "LinuxTimerManager.h"
#include <QDebug>
#include <QMetaType>
#include <sys/syscall.h>


#define  MAX_LOCK_TIME  10
#define  MAX_REQUEST_LOCK_TIME  2*1000


QMap<timer_t,callBackFun > LinuxTimerManager::myTimerMap;
QMap<timer_t,QString > LinuxTimerManager::myTimerNameMap;
QQueue<timer_t> LinuxTimerManager::myTimerRequestQueue;
QMutex LinuxTimerManager::timerRequestQueueMutex;

QMutex  LinuxTimerManager::myMutex;
LinuxTimerManager *LinuxTimerManager::self=NULL;
QMutex LinuxTimerManager::tongBuMutex;

LinuxTimerManager *LinuxTimerManager::instance()
{
    if(self==NULL)
    {
        self = new LinuxTimerManager();
    }
    return self;
}

void LinuxTimerManager::recvSigFromSystem( sigval  v)
{
    timerRequestQueueMutex.lock();
    myTimerRequestQueue.enqueue(*(timer_t *)v.sival_ptr);
    timerRequestQueueMutex.unlock();
    QString pluginName;
    if(myTimerNameMap.contains(*(timer_t *)v.sival_ptr))
    {
        pluginName =  myTimerNameMap.value(*(timer_t *)v.sival_ptr);
        qDebug()<<"[#LinuxTimerManager::recvSigFromSystem#]当前定时器个数:"<< myTimerRequestQueue.size()<<"pluginName="<< pluginName;
    }
    return;
}


void LinuxTimerManager::recvSystemTimerSlot(timer_t v)
{
    if(timerRequestQueueMutex.tryLock(10)==false)
    {
        tongBuMutex.unlock();
        return;
    }
    if(!myTimerRequestQueue.isEmpty())
    {
        myTimerRequestQueue.dequeue();
    }
    else
    {
        timerRequestQueueMutex.unlock();
        tongBuMutex.unlock();
        return;
    }
    timerRequestQueueMutex.unlock();

    QString pluginName;
    if(myTimerNameMap.contains(v))
    {
        pluginName =  myTimerNameMap.value(v);
    }
    if(myMutex.tryLock(MAX_LOCK_TIME)==false)
    {
        qDebug()<<"ERROR!!![#LinuxTimerManager::recvSystemTimerSlot#]pluginName="<<pluginName<<"other thread is lock!";
        tongBuMutex.unlock();
        return;
    }
    if(myTimerMap.contains(v))
    {
        callBackFun fun = myTimerMap.value(v);
        myMutex.unlock();  //一定要提前释放锁资源
        qDebug()<<"[#LinuxTimerManager::recvSystemTimerSlot#]正在处理定时器 pluginName="<<pluginName;
        struct itimerspec it;
        timer_gettime(v,&it);
        if(it.it_value.tv_sec != 0)
        {
            fun(v);
        }
        tongBuMutex.unlock();
        return;
    }
    myMutex.unlock();
    tongBuMutex.unlock();
    return;
}

void *LinuxTimerManager::TimerRequestQueueThread(void *msg)
{
    qDebug() <<"TimerRequestQueueThread 线程ID:"<<syscall(SYS_gettid);
    LinuxTimerManager *pLinuxTimerManager = (LinuxTimerManager *)msg;
    while(true)
    {
        if(timerRequestQueueMutex.tryLock(10)==false)
        {
            usleep(10*1000);
            continue;
        }

        if(myTimerRequestQueue.empty())
        {
            timerRequestQueueMutex.unlock();
            usleep(10*1000);
            continue;
        }
        else
        {
                timer_t v = myTimerRequestQueue.head();
                timerRequestQueueMutex.unlock();
                emit pLinuxTimerManager->sendSig(v);
                tongBuMutex.lock();
        }
        usleep(10*1000);
    }
    return NULL;
}


LinuxTimerManager::LinuxTimerManager()
{
    qRegisterMetaType<timer_t>("timer_t");
    connect(this, &LinuxTimerManager::sendSig, this, &LinuxTimerManager::recvSystemTimerSlot);
    pthread_t tid1;
    pthread_create(&tid1, NULL, TimerRequestQueueThread, this);
}

LinuxTimerManager::~LinuxTimerManager()
{

}

int LinuxTimerManager::initLinuxTimer(timer_t &yourTimerId, callBackFun fun,QString pluginName)
{
    LinuxTimerManager::instance();

    struct sigevent evp;
    memset(&evp, 0, sizeof(struct sigevent));	//清零初始化

    evp.sigev_value.sival_ptr = &yourTimerId;
    evp.sigev_notify = SIGEV_THREAD;		//线程通知的方式，派驻新线程
    evp.sigev_notify_function = recvSigFromSystem;            //线程函数地址


    if (timer_create(CLOCKID, &evp, &yourTimerId) == -1)
    {
        qDebug()<<"[#LinuxTimerTest::initLinuxTimer#] can not create  pluginName="<<pluginName;
        return LINUX_TIMER_RETURN_ERROR;
    }
    if(myMutex.tryLock(MAX_LOCK_TIME)==false)
    {
        qDebug()<<"ERROR!!![#LinuxTimerManager::initLinuxTimer#]pluginName="<<pluginName <<"other thread is lock!";
        return LINUX_TIMER_RETURN_ERROR;
    }

    if(!myTimerMap.contains(yourTimerId))
    {
        myTimerMap.insert(yourTimerId,fun);
    }
    if(!myTimerNameMap.contains(yourTimerId))
    {
        myTimerNameMap.insert(yourTimerId,pluginName);
    }
    myMutex.unlock();
    return LINUX_TIMER_RETURN_OK;
}

int LinuxTimerManager::startLinuxTimer(timer_t &yourTimerId, int sec, int nsec)
{
    struct itimerspec it;
    it.it_interval.tv_sec =sec;	//间隔1s
    it.it_interval.tv_nsec = nsec;
    it.it_value.tv_sec = sec;
    it.it_value.tv_nsec = nsec;

    if (timer_settime(yourTimerId, 0, &it, NULL) == -1)
    {
        QString pluginName;
        if(myTimerNameMap.contains(yourTimerId))
        {
            pluginName =  myTimerNameMap.value(yourTimerId);
        }
        qDebug()<<"[#LinuxTimerManager::startLinuxTimer#] can not startimer pluginName="<<pluginName;
        return LINUX_TIMER_RETURN_ERROR;
    }
    return LINUX_TIMER_RETURN_OK;
}

int LinuxTimerManager::startSingleLinuxTimer(timer_t &yourTimerId, int sec, int nsec)
{
    struct itimerspec it;
    it.it_interval.tv_sec =0;	//间隔1s
    it.it_interval.tv_nsec = 0;
    it.it_value.tv_sec = sec;
    it.it_value.tv_nsec = nsec;
    if (timer_settime(yourTimerId, 0, &it, NULL) == -1)
    {
        QString pluginName;
        if(myTimerNameMap.contains(yourTimerId))
        {
            pluginName =  myTimerNameMap.value(yourTimerId);
        }
        qDebug()<<"[#LinuxTimerManager::startSingleLinuxTimer#] can not start timer pluginName="<<pluginName;
        return LINUX_TIMER_RETURN_ERROR;
    }

    return LINUX_TIMER_RETURN_OK;
}

int LinuxTimerManager::stopLinuxTimer(timer_t &yourTimerId)
{
    struct itimerspec it;
    it.it_interval.tv_sec =0;	//间隔1s
    it.it_interval.tv_nsec = 0;
    it.it_value.tv_sec = 0;
    it.it_value.tv_nsec = 0;

    if (timer_settime(yourTimerId, 0, &it, NULL) == -1)
    {
        QString pluginName;
        if(myTimerNameMap.contains(yourTimerId))
        {
            pluginName =  myTimerNameMap.value(yourTimerId);
            qDebug()<<"[#LinuxTimerManager::stopLinuxTimer#] can not stop pluginName="<<pluginName;
        }

        return LINUX_TIMER_RETURN_ERROR;
    }
    return LINUX_TIMER_RETURN_OK;
}

int LinuxTimerManager::closeLinuxTimer(timer_t &yourTimerId)
{
    if(myMutex.tryLock(MAX_LOCK_TIME)==false)
        return LINUX_TIMER_RETURN_ERROR;

    if(myTimerMap.contains(yourTimerId))
    {
        myTimerMap.remove(yourTimerId);
    }
    myMutex.unlock();
    timer_delete(yourTimerId);
    return LINUX_TIMER_RETURN_OK;
}

