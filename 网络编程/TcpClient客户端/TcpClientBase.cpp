#include "TcpClientBase.h"
#include <QTcpSocket>
#include <QNetworkProxy>
#include <QNetworkInterface>
#include <QTimer>
#include "GeneralHelp.h" //公共的辅助类，这里主要是写log

TcpClientBase::TcpClientBase()
    : m_bIsConnected(false)
{
    m_pSocket = new QTcpSocket(this);
    connect(this, &TcpClientBase::signalRecv, this, &TcpClientBase::slotRecv);
    connect(m_pSocket, &QTcpSocket::readyRead, this, &TcpClientBase::slotRecvMsg);
    connect(m_pSocket, &QTcpSocket::connected, this, &TcpClientBase::slotConnected);
    connect(m_pSocket, &QTcpSocket::disconnected, this, &TcpClientBase::slotDisConnected);
	connect(m_pSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred), this, &TcpClientBase::SlotSocketError);

    m_pConnectTimer = new QTimer();
    m_pConnectTimer->setSingleShot(false);
    connect(m_pConnectTimer, &QTimer::timeout, this, [=]{
        WriteLogWarning("连接网关超时，正在重连..."+ m_strIp +", " + to_string(m_nPort));
        connectSvr(m_strIp.c_str(), m_nPort);
    });
}

TcpClientBase::~TcpClientBase()
{
    DELETEOBJECT(m_pSocket);
}

// 连接tcp服务器
int TcpClientBase::connectSvr(const char* pszIp, int nPort)
{
    if (nullptr == pszIp || !Helper->isVaildIp(pszIp))
        return EnumResult_ConnectFail;
    
    m_nPort = nPort;
    m_strIp = pszIp;

    m_pSocket->setProxy(QNetworkProxy::NoProxy);
    m_pSocket->connectToHost(QHostAddress(pszIp), quint16(nPort));
    if (!m_pSocket->waitForConnected(3 * 1000))      // 5秒钟的超时
    {
        if (!m_pConnectTimer->isActive())
            m_pConnectTimer->start(5000);
        return EnumResult_TimeOut;
    }
    else
    {
        m_pConnectTimer->stop();
    }

    return EnumResult_OK;
}

// 断开tcp服务器
int TcpClientBase::disConnect()
{
    m_pSocket->disconnectFromHost();
    return EnumResult_OK;
}

// 发送数据
int TcpClientBase::send(const char* pszData, int nLen)
{
    if(nullptr == pszData || 0 == nLen)
        return EnumResult_DataNone;
        
    if (!m_bIsConnected)
        return EnumResult_ConnectFail;
    
    qint64 nSendLen = m_pSocket->write(pszData, nLen);
    m_pSocket->flush();
    return static_cast<int>(nSendLen);
}

// 接收数据
int TcpClientBase::recv(const char* pszData, int nLen)
{
    if(nullptr == pszData || 0 == nLen)
        return false;
    return true;
}

// 获取消息的信号
void TcpClientBase::slotRecvMsg()
{
    QByteArray byteArray = m_pSocket->readAll();
    emit signalRecv(byteArray);
}

// tcp 读取的错误信息
void TcpClientBase::slotReadError(QAbstractSocket::SocketError)
{

}

// tcp 断开连接信号
void TcpClientBase::slotConnected()
{
    m_bIsConnected = true;
}

// tcp 断开连接信号
void TcpClientBase::slotDisConnected()
{
    m_bIsConnected = false;
}

// tcp 重连信号
void TcpClientBase::slotReconnection()
{

}

// tcp 数据分发
void TcpClientBase::slotRecv(QByteArray byteArray)
{
    const char* pszData = byteArray.data();
    int nLen = byteArray.length();
    recv(pszData, nLen);
}

void TcpClientBase::slotSocketError(QAbstractSocket::SocketError)
{
	QString data = m_pSocket->errorString();
	qDebug() << __LINE__ << __FUNCTION__ << data;
}

