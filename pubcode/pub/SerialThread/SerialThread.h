#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QObject>
#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QMutex>

class SerialThread : public QThread
{
    Q_OBJECT
public:
    explicit SerialThread();
    ~SerialThread();


public:
    bool     OpenCom(QString portName, int baudRate, QSerialPort::DataBits dataBits = QSerialPort::Data8, QSerialPort::Parity parity = QSerialPort::NoParity, QSerialPort::StopBits stopBits = QSerialPort::OneStop, QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl);
    bool     CloseCom();
    qint64   WriteCom(QByteArray& array);
    bool     ComStatue();

    QString    m_PortName;
    int        m_BaudRate;

    QTimer    *m_RetSetComTimer;

signals:
    void sigRecvOneFrame(QByteArray frame);
    void sig2ResetCheckTimer();
public slots:
    void ReadCom();
    bool ReOpenCom();

    void slResetCheckTimer();
protected:
    void run();

private:
    QSerialPort *m_Com;
    QByteArray m_readBuff;
    QMutex m_readMutex;

    bool threadRunFlag;

    void     DestroyCom();
    static void *SerialRunHandle(void *pData);

    bool m_bOpenOk;

};

#endif // SERIALTHREAD_H

