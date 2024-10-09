#ifndef LINUXTIMERMANAGER_H
#define LINUXTIMERMANAGER_H

//#include "CommonBus.h"
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define CLOCKID CLOCK_REALTIME
#define  LINUX_TIMER_RETURN_OK  0
#define  LINUX_TIMER_RETURN_ERROR  -1


#include <QMap>
#include <QObject>
#include <QMutex>
#include <QQueue>

typedef    void (*callBackFun )(timer_t );



class LinuxTimerManager : public QObject
{
    Q_OBJECT
public:
    static LinuxTimerManager *instance();
    static LinuxTimerManager *self;
    static void recvSigFromSystem( sigval v);
    LinuxTimerManager();
    ~LinuxTimerManager();

    static int initLinuxTimer(timer_t &yourTimerId,callBackFun fun,QString pluginName);
    static int startLinuxTimer(timer_t &yourTimerId,int sec,int nsec );
    static int startSingleLinuxTimer(timer_t &yourTimerId,int sec, int nsec);

    static int stopLinuxTimer(timer_t &yourTimerId);
    static int closeLinuxTimer(timer_t &yourTimerId);

    static QMap<timer_t,callBackFun> myTimerMap;
    static QMap<timer_t,QString > myTimerNameMap;
    static QMutex  myMutex;


    static QQueue<timer_t> myTimerRequestQueue;
    static void *TimerRequestQueueThread(void *msg);
    static QMutex timerRequestQueueMutex;
    static QMutex tongBuMutex;

signals:
     void sendSig(timer_t v);

public slots:
    void recvSystemTimerSlot(timer_t v);
};

#endif // LINUXTIMERMANAGER_H
