#include "SerialThread.h"

#include <unistd.h>
#include <sys/syscall.h>
#include <QDebug>

SerialThread::SerialThread():
    m_PortName(""),
    m_BaudRate(0),
    m_Com(nullptr)
{

    m_RetSetComTimer = new QTimer();
//    connect(m_RetSetComTimer, &QTimer::timeout, this, &SerialThread::ReOpenCom);
//    connect(this, &SerialThread::sig2ResetCheckTimer, this, &SerialThread::slResetCheckTimer);

    threadRunFlag = true; //工作线程循环标识
    pthread_t myThread1;
    pthread_create(&myThread1, nullptr, SerialRunHandle, this);


}
SerialThread::~SerialThread()
{
    DestroyCom();
}

//打开串口
bool SerialThread::OpenCom(QString portName, int baudRate, QSerialPort::DataBits dataBits, QSerialPort::Parity parity, QSerialPort::StopBits stopBits, QSerialPort::FlowControl flowControl)
{
    QString strLog;
    bool bReturn = false;

    m_PortName = portName;
    m_BaudRate = baudRate;

    if(nullptr == m_Com)
    {
        m_Com = new QSerialPort();
    }

    m_Com->setPortName(m_PortName);
    m_Com->setBaudRate(m_BaudRate);
    m_Com->setDataBits(dataBits);
    m_Com->setParity(parity);
    m_Com->setStopBits(stopBits);
    m_Com->setFlowControl(flowControl);
    bReturn = m_Com->open(QIODevice::ReadWrite);
    if(bReturn)
    {
        strLog = tr("串口%1打开成功, 波特率：%2，数据位：%3，校验位：%4，停止位：%5，流控制：%6").arg(m_Com->portName()).arg(m_Com->baudRate()).arg(m_Com->dataBits()).arg(m_Com->parity()).arg(m_Com->stopBits()).arg(m_Com->flowControl());
        connect(m_Com, &QSerialPort::readyRead, this, &SerialThread::ReadCom);
    }
    else
    {
        strLog = tr("串口%1打开失败, 波特率：%2, %3").arg(m_PortName).arg(m_BaudRate).arg(m_Com->errorString());
    }
    m_RetSetComTimer->start(1000*60);
    qDebug() << strLog;

    return bReturn;
}
//关闭串口
bool SerialThread::CloseCom()
{
    if (m_Com->isOpen())
    {
        m_Com->close();
    }
    if (m_Com !=nullptr)
    {
        m_Com->deleteLater();
        m_Com =nullptr;
    }

    return true;
}
//重新打开串口
bool SerialThread::ReOpenCom()
{
    qDebug() << "长时间未收到数据，重置串口";
    bool ok;

    CloseCom();

    ok = OpenCom(m_PortName,m_BaudRate);
    return ok;
}

void SerialThread::slResetCheckTimer()
{
    m_RetSetComTimer->stop();
    m_RetSetComTimer->start(60*1000);
}
qint64 SerialThread::WriteCom(QByteArray &array)
{
    if(nullptr == m_Com)
    {
        return 0;
    }

    static QMutex mutex;
    mutex.lock();

    QString strLog;
    int len = array.length();
    QString str = array.toHex().toUpper();

    qint64 m;
    m=m_Com->write(array);

    strLog = tr("发出[%1][实际写入:%2]: %3").arg(len).arg(m).arg(str);
    qDebug() << strLog;

    mutex.unlock();
    return  m;
}
void SerialThread::ReadCom()
{
    if(nullptr == m_Com)
    {
        return ;
    }
    m_readMutex.lock();
    m_readBuff += m_Com->readAll();
    m_readMutex.unlock();
}

void SerialThread::run()
{
    exec();
}

//线程函数,解析提取一帧数据
void *SerialThread::SerialRunHandle(void *pData)
{
    qDebug() << "SerialRunHandle 线程ID:" << QString::number(syscall(SYS_gettid));
    SerialThread *pSerialThread = reinterpret_cast<SerialThread*>(pData);
    while(pSerialThread->threadRunFlag)
    {
        if(pSerialThread->m_readBuff.isEmpty()==true)
        {
            usleep(1000*2);
            continue;
        }

        if(pSerialThread->m_readMutex.tryLock(1*1000)==true)
        {
            emit pSerialThread->sig2ResetCheckTimer();
            QByteArray strKeyVal = pSerialThread->m_readBuff;
            emit pSerialThread->sigRecvOneFrame(strKeyVal);
            pSerialThread->m_readBuff.clear();
            pSerialThread->m_readMutex.unlock();
        }
        else
        {
            usleep(1000*1); //避免性线占用100%
        }
    }

    qDebug("工作线程 SerialThread::SerialRunHandle(void *pData)  退出....");

    return nullptr;
}

void SerialThread::DestroyCom()
{
    if(nullptr != m_RetSetComTimer)
    {
        m_RetSetComTimer->stop();
        m_RetSetComTimer->deleteLater();
        m_RetSetComTimer = nullptr;
    }
    threadRunFlag  = false; //串口关闭时，读串口数据的线程应该先退出
    sleep(2);
    CloseCom();

    qDebug("串口已关闭");
}

bool SerialThread::ComStatue()
{
    return m_Com->isOpen();
}
